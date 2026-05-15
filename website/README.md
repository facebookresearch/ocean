# Ocean Documentation Website

This website is built using a Python build script that converts Markdown documentation into a static HTML site. No npm, Node.js, or JavaScript build tools are required.

### Prerequisites

- Python 3.8+

The `markdown` Python package is vendored in `third_party/` — no pip install needed.

### Build

From inside the `website/` directory:

```bash
python build.py
```

This fetches remote build guides from GitHub, converts all Markdown docs to HTML, and outputs the site to the `build/` directory.

### Local Development

To build and preview the site locally:

```bash
python build.py --serve
```

This builds the site and starts a local HTTP server at `http://localhost:3000/ocean/`.

### Options

```bash
python build.py --skip-fetch       # Skip fetching build guides from GitHub
python build.py --with-doxygen     # Generate Doxygen API reference
python build.py --port 8080        # Use a custom port for the local server
```

### Doxygen API Reference

The C++ API reference is generated separately using Doxygen:

```bash
python build.py --with-doxygen
```

This requires `doxygen` to be installed and available in your PATH.

### Project Structure

```
website/
  build.py              # Build script
  docs/                 # Markdown documentation source files
  static/img/           # Images and static assets
  templates/            # HTML page templates
  assets/               # CSS, JavaScript, and vendor files
  build/                # Generated output (gitignored)
```
