/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

(function () {
  'use strict';

  // ---- Theme Toggle ----

  function setTheme(theme) {
    document.documentElement.setAttribute('data-theme', theme);
    localStorage.setItem('ocean-theme', theme);
  }

  // ---- DOM Ready ----
  // Theme is initialized by an inline <script> in <head> to prevent FOUC.
  // This file is loaded with defer, so DOMContentLoaded is guaranteed.

  document.addEventListener('DOMContentLoaded', function () {

    // Theme toggle button
    var themeBtn = document.querySelector('.theme-toggle');
    if (themeBtn) {
      themeBtn.addEventListener('click', function () {
        var current = document.documentElement.getAttribute('data-theme');
        setTheme(current === 'dark' ? 'light' : 'dark');
      });
    }

    // ---- Sidebar Toggle (Mobile) ----

    var hamburger = document.querySelector('.hamburger');
    var sidebar = document.querySelector('.sidebar');
    var overlay = document.querySelector('.sidebar-overlay');

    function closeSidebar() {
      if (sidebar) sidebar.classList.remove('sidebar--open');
      if (overlay) overlay.classList.remove('sidebar-overlay--visible');
    }

    if (hamburger) {
      hamburger.addEventListener('click', function () {
        if (sidebar) sidebar.classList.toggle('sidebar--open');
        if (overlay) overlay.classList.toggle('sidebar-overlay--visible');
      });
    }

    if (overlay) {
      overlay.addEventListener('click', closeSidebar);
    }

    // ---- Collapsible Sidebar Categories ----

    var categoryHeaders = document.querySelectorAll('.sidebar__category-header');
    categoryHeaders.forEach(function (header) {
      header.addEventListener('click', function () {
        var category = header.parentElement;
        category.classList.toggle('sidebar__category--expanded');
      });
    });

    // Auto-expand category containing the active page
    var activeLink = document.querySelector('.sidebar__link--active');
    if (activeLink) {
      var parent = activeLink.closest('.sidebar__category');
      while (parent) {
        parent.classList.add('sidebar__category--expanded');
        parent = parent.parentElement.closest('.sidebar__category');
      }
    }

    // ---- Tabs ----

    document.querySelectorAll('.tabs').forEach(function (tabGroup) {
      var tabs = tabGroup.querySelectorAll('.tabs__tab');
      var panels = tabGroup.querySelectorAll('.tabs__panel');
      var groupId = tabGroup.getAttribute('data-group');

      tabs.forEach(function (tab) {
        tab.addEventListener('click', function () {
          var value = tab.getAttribute('data-tab');

          // Deactivate all in this group
          tabs.forEach(function (t) { t.classList.remove('tabs__tab--active'); });
          panels.forEach(function (p) { p.classList.remove('tabs__panel--active'); });

          // Activate clicked
          tab.classList.add('tabs__tab--active');
          var panel = tabGroup.querySelector('.tabs__panel[data-tab="' + value + '"]');
          if (panel) panel.classList.add('tabs__panel--active');

          // Sync across groups with same groupId
          if (groupId) {
            document.querySelectorAll('.tabs[data-group="' + groupId + '"]').forEach(function (otherGroup) {
              if (otherGroup === tabGroup) return;
              otherGroup.querySelectorAll('.tabs__tab').forEach(function (t) {
                t.classList.toggle('tabs__tab--active', t.getAttribute('data-tab') === value);
              });
              otherGroup.querySelectorAll('.tabs__panel').forEach(function (p) {
                p.classList.toggle('tabs__panel--active', p.getAttribute('data-tab') === value);
              });
            });

            // Update URL query string
            var url = new URL(window.location);
            url.searchParams.set(groupId, value);
            history.replaceState(null, '', url);
          }
        });
      });
    });

    // Restore tab selection from URL query string
    var params = new URLSearchParams(window.location.search);
    params.forEach(function (value, key) {
      document.querySelectorAll('.tabs[data-group="' + key + '"]').forEach(function (group) {
        var tab = group.querySelector('.tabs__tab[data-tab="' + value + '"]');
        if (tab) tab.click();
      });
    });

    // ---- Wrap tables in scroll containers ----

    document.querySelectorAll('.content table').forEach(function (table) {
      if (!table.parentElement.classList.contains('table-wrapper')) {
        var wrapper = document.createElement('div');
        wrapper.className = 'table-wrapper';
        table.parentNode.insertBefore(wrapper, table);
        wrapper.appendChild(table);
      }
    });
  });
})();
