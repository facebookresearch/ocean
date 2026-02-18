# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

"""Progress display with TUI-like updates for parallel builds.

Uses the 'rich' library for cross-platform terminal output.
Install with: pip install rich

Design:
- Completed jobs print immediately as permanent output (scrolling up)
- Active jobs shown in Live region at bottom (updates in place)
- Worker threads update internal data structures (with locking)
- Rich's refresh thread renders the active jobs display
"""

from __future__ import annotations

import os
import threading
import time
from dataclasses import dataclass, field
from enum import Enum
from typing import Dict, List, Optional

# Enable Windows Virtual Terminal mode for ANSI escape sequence support.
# This must be done before importing rich to ensure proper initialization.
# If the mode cannot be enabled (e.g., output is piped or the console doesn't
# support it), we record this so that Rich can be told not to emit ANSI codes.
_WIN_VT_ENABLED = True  # Assume True for non-Windows or when not needed

if os.name == "nt":
    _WIN_VT_ENABLED = False  # Assume failure; set True only on success
    try:
        import ctypes
        from ctypes import wintypes

        kernel32 = ctypes.windll.kernel32
        ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004

        for std_handle_id in (-11, -12):  # STD_OUTPUT_HANDLE, STD_ERROR_HANDLE
            handle = kernel32.GetStdHandle(std_handle_id)
            mode = wintypes.DWORD()
            if kernel32.GetConsoleMode(handle, ctypes.byref(mode)):
                new_mode = mode.value | ENABLE_VIRTUAL_TERMINAL_PROCESSING
                if kernel32.SetConsoleMode(handle, new_mode):
                    _WIN_VT_ENABLED = True
    except Exception:
        pass

# Try to import rich, fall back to simple output if not available
try:
    from rich.console import Console
    from rich.live import Live
    from rich.text import Text

    RICH_AVAILABLE = True
except ImportError:
    RICH_AVAILABLE = False


class BuildPhase(Enum):
    """Build phases."""

    FETCHING = "Fetching"
    CONFIGURING = "Configuring"
    BUILDING = "Building"
    INSTALLING = "Installing"
    REORGANIZING = "Finalizing"
    COMPLETE = "Complete"
    FAILED = "Failed"


@dataclass
class JobStatus:
    """Status of a single build job."""

    library: str
    platform: str
    config: str
    link: str
    phase: BuildPhase
    start_time: float = field(default_factory=time.time)
    end_time: Optional[float] = None
    error_message: Optional[str] = None
    completion_counter: Optional[str] = None  # Counter string at completion time

    @property
    def duration_seconds(self) -> float:
        """Get duration in seconds."""
        end = self.end_time if self.end_time else time.time()
        return end - self.start_time


def _format_duration(seconds: float) -> str:
    """Format duration as MM:SS or Xs."""
    mins = int(seconds) // 60
    secs = int(seconds) % 60
    if mins > 0:
        return f"{mins}:{secs:02d}"
    return f"{secs}s"


class _ActiveJobsRenderer:
    """Helper class that Rich calls on each refresh to get active jobs display."""

    def __init__(self, progress_display: "ProgressDisplay"):
        self._progress = progress_display

    def __rich__(self) -> Text:
        """Called by Rich on each refresh."""
        return self._progress._build_active_display()


class ProgressDisplay:
    """Thread-safe progress display with TUI updates.

    Completed jobs print as permanent output (scrolling up).
    Active jobs shown in Live region at bottom.
    """

    # Configuration
    MAX_ACTIVE_JOBS = 32  # Maximum active jobs to display

    def __init__(self, total_jobs: int, use_color: bool = True):
        """Initialize progress display."""
        self.total_jobs = total_jobs
        self.completed_count = 0
        self.failed_count = 0
        self._lock = threading.RLock()  # Reentrant lock for nested acquisition

        # Job tracking
        self._active_jobs: Dict[str, JobStatus] = {}

        # Rich components
        self._console: Optional[Console] = None
        self._live: Optional[Live] = None
        self._use_rich = RICH_AVAILABLE and use_color

        if self._use_rich:
            if os.name == "nt":
                # Only use ANSI codes if Virtual Terminal Processing is active.
                # When VT is not enabled (e.g., older cmd.exe, piped output),
                # raw escape sequences would litter the output.
                self._console = Console(
                    force_terminal=_WIN_VT_ENABLED,
                    no_color=not _WIN_VT_ENABLED,
                )
            else:
                self._console = Console()
        elif not RICH_AVAILABLE:
            print("Note: Install 'rich' for better progress display: pip install rich")

    def _job_key(self, library: str, platform: str, config: str, link: str) -> str:
        """Get unique key for a job."""
        return f"{library}:{platform}:{config}:{link}"

    def _format_counter_at(self, count: int) -> str:
        """Format the job counter for a specific count."""
        pct = (count * 100) // self.total_jobs if self.total_jobs > 0 else 0
        return f"[{count:3d}/{self.total_jobs:3d} {pct:3d}%]"

    def _format_counter(self) -> str:
        """Format the job counter with current completed count."""
        return self._format_counter_at(self.completed_count)

    def _build_active_display(self) -> Text:
        """Build the active jobs display.

        Called by Rich's refresh thread. Must acquire lock to read data.
        """
        with self._lock:
            base_count = self.completed_count
            output = Text()

            # Add active jobs (up to MAX_ACTIVE_JOBS)
            active_list = list(self._active_jobs.values())[: self.MAX_ACTIVE_JOBS]

            if not active_list:
                return output

            # First pass: calculate max widths for alignment
            job_tags = []
            phases = []
            timers = []
            for job in active_list:
                job_tags.append(
                    f"[{job.library}, {job.platform}, {job.config}, {job.link}]"
                )
                phases.append(f"{job.phase.value}...")
                timers.append(f"[{_format_duration(job.duration_seconds)}]")

            max_job_tag = max(len(t) for t in job_tags)
            max_phase = max(len(p) for p in phases)
            max_timer = max(len(t) for t in timers)

            # Second pass: build output with alignment
            for i, _job in enumerate(active_list):
                if i > 0:
                    output.append("\n")
                # Each active job gets a pseudo-index: completed + row + 1
                counter = self._format_counter_at(base_count + i + 1)
                output.append(counter, style="dim")
                output.append(f" {job_tags[i]:<{max_job_tag}}", style="cyan")
                output.append(f": {phases[i]:<{max_phase}}", style="yellow")
                output.append(f" {timers[i]:>{max_timer}}", style="dim")

            return output

    def _start_live(self) -> None:
        """Start the live display."""
        if self._use_rich and self._console and not self._live:
            # Use _ActiveJobsRenderer so Rich calls _build_active_display() on each refresh
            renderer = _ActiveJobsRenderer(self)
            self._live = Live(
                renderer,
                console=self._console,
                refresh_per_second=10,  # Faster refresh for responsive counter updates
                transient=True,  # Disappears when stopped
            )
            self._live.start()

    def _stop_live(self) -> None:
        """Stop the live display."""
        if self._live:
            self._live.stop()
            self._live = None

    def _print_job_completion(self, job: JobStatus) -> None:
        """Print a completed job as permanent output above the live region."""
        counter = job.completion_counter or self._format_counter()
        job_tag = f"[{job.library}, {job.platform}, {job.config}, {job.link}]"
        duration = _format_duration(job.duration_seconds)

        if self._use_rich:
            output = Text()
            output.append(counter, style="dim")
            output.append(f" {job_tag}", style="cyan")
            output.append(": ")
            if job.phase == BuildPhase.FAILED:
                output.append("FAIL", style="bold red")
                if job.error_message:
                    error_display = (
                        job.error_message[:100] + "..."
                        if len(job.error_message) > 100
                        else job.error_message
                    )
                    output.append(f" - {error_display}", style="red")
            else:
                output.append("OK", style="bold green")
            output.append(f" [{duration}]", style="dim")

            # Use live.console.print() to print above the live region
            if self._live:
                self._live.console.print(output)
            elif self._console:
                self._console.print(output)
        else:
            if job.phase == BuildPhase.FAILED:
                print(f"{counter} {job_tag}: FAIL")
            else:
                print(f"{counter} {job_tag}: OK [{duration}]")

    def start_job(
        self, library: str, platform: str, config: str, link: str, phase: BuildPhase
    ) -> None:
        """Start tracking a new job."""
        key = self._job_key(library, platform, config, link)
        should_start_live = False

        with self._lock:
            self._active_jobs[key] = JobStatus(
                library=library,
                platform=platform,
                config=config,
                link=link,
                phase=phase,
                start_time=time.time(),
            )

            # Check if we need to start live display
            if len(self._active_jobs) == 1 and not self._live:
                should_start_live = True

        # Start live display outside the lock to avoid deadlock
        if should_start_live:
            self._start_live()

    def update_phase(
        self, library: str, platform: str, config: str, link: str, phase: BuildPhase
    ) -> None:
        """Update a job's phase."""
        key = self._job_key(library, platform, config, link)

        with self._lock:
            if key in self._active_jobs:
                self._active_jobs[key].phase = phase

    def complete_job(
        self,
        library: str,
        platform: str,
        config: str,
        link: str,
        duration_seconds: float,
    ) -> None:
        """Mark a job as successfully complete."""
        key = self._job_key(library, platform, config, link)
        job_to_print = None

        with self._lock:
            self.completed_count += 1

            if key not in self._active_jobs:
                return

            # Move from active to completed
            job = self._active_jobs.pop(key)
            job.phase = BuildPhase.COMPLETE
            job.end_time = time.time()
            job.completion_counter = (
                self._format_counter()
            )  # Record counter at completion
            job_to_print = job

        # Print outside the lock to avoid deadlock with Rich's refresh thread
        if job_to_print:
            self._print_job_completion(job_to_print)

    def fail_job(
        self,
        library: str,
        platform: str,
        config: str,
        link: str,
        error_message: str,
    ) -> None:
        """Mark a job as failed."""
        key = self._job_key(library, platform, config, link)
        job_to_print = None

        with self._lock:
            self.completed_count += 1
            self.failed_count += 1

            if key not in self._active_jobs:
                return

            # Move from active to completed
            job = self._active_jobs.pop(key)
            job.phase = BuildPhase.FAILED
            job.end_time = time.time()
            job.error_message = error_message
            job.completion_counter = (
                self._format_counter()
            )  # Record counter at completion
            job_to_print = job

        # Print outside the lock to avoid deadlock with Rich's refresh thread
        if job_to_print:
            self._print_job_completion(job_to_print)

    def print_level_header(
        self,
        level: int,
        lib_count: int,
        target_count: int,
        job_count: int,
        libraries: List[str],
    ) -> None:
        """Print a level header."""
        with self._lock:
            # Stop live display
            self._stop_live()

            # Clear active jobs for new level
            self._active_jobs.clear()

            # Print header
            header = f"{'═' * 70}"
            level_info = f"Level {level}: {lib_count} libraries × {target_count} targets = {job_count} jobs"
            lib_list = f"  Libraries: {', '.join(sorted(libraries))}"

            if self._use_rich and self._console:
                self._console.print(header)
                self._console.print(level_info)
                self._console.print(lib_list)
                self._console.print(header)
            else:
                print(header)
                print(level_info)
                print(lib_list)
                print(header)

    def print_summary(self) -> None:
        """Print final summary."""
        with self._lock:
            # Stop live
            self._stop_live()

            if self._use_rich and self._console:
                self._console.print()
                if self.failed_count == 0:
                    self._console.print(
                        f"✓ All {self.completed_count} builds completed successfully",
                        style="bold green",
                    )
                else:
                    self._console.print(
                        f"✗ {self.failed_count} builds failed, "
                        f"{self.completed_count - self.failed_count} succeeded",
                        style="bold red",
                    )
            else:
                print()
                if self.failed_count == 0:
                    print(f"All {self.completed_count} builds completed successfully")
                else:
                    print(
                        f"{self.failed_count} builds failed, "
                        f"{self.completed_count - self.failed_count} succeeded"
                    )


def format_duration(seconds: float) -> str:
    """Format duration as MM:SS or Xs."""
    return _format_duration(seconds)
