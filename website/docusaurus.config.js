/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Force staticdocs rebuild #1

const {fbContent, fbInternalOnly} = require('docusaurus-plugin-internaldocs-fb/internal');
const repoUrl = 'https://github.com/facebookresearch/ocean';

import {themes as prismThemes} from 'prism-react-renderer';

// With JSDoc @type annotations, IDEs can provide config autocompletion
/** @type {import('@docusaurus/types').DocusaurusConfig} */
(module.exports = {
  title: 'Ocean',
  tagline: 'A C++ Framework For Computer Vision (CV) And Augmented Reality (AR) Applications',
  url: 'https://facebookresearch.github.io',
  baseUrl: '/ocean/',
  onBrokenLinks: 'throw',
  onBrokenMarkdownLinks: 'throw',
  trailingSlash: true,
  favicon: 'img/ocean_favicon.ico',
  organizationName: 'facebookresearch',
  projectName: 'ocean',
  customFields: {
    fbRepoName: 'fbsource',
    ossRepoPath: 'xplat/ocean/website',
  },

  plugins: [
    [
      'docusaurus-plugin-remote-content',
      {
        name: 'building-docs',
        sourceBaseUrl: 'https://raw.githubusercontent.com/facebookresearch/ocean/main/',
        outDir: 'docs/building',
        documents: [
          'building_for_android.md',
          'building_for_ios.md',
          'building_for_linux.md',
          'building_for_macos.md',
          'building_for_meta_quest.md',
          'building_for_windows.md',
        ],
        modifyContent(filename, content) {
          // Convert relative links to absolute GitHub URLs
          let modifiedContent = content
            .replace(/\]\((?!https?:\/\/|#)([^)]+)\)/g, '](https://github.com/facebookresearch/ocean/blob/main/$1)');

          // Remove the first H1 heading since we add our own title in frontmatter
          modifiedContent = modifiedContent.replace(/^# .+\n\n?/, '');

          // Escape curly braces in code blocks to prevent JSX interpretation
          // Replace ${...} with {'${...}'} to escape shell variables
          modifiedContent = modifiedContent.replace(/\$\{([^}]+)\}/g, "{'{$$$1}'}");

          const platformMap = {
            'building_for_android.md': { title: 'Building for Android', position: 2 },
            'building_for_ios.md': { title: 'Building for iOS', position: 3 },
            'building_for_linux.md': { title: 'Building for Linux', position: 4 },
            'building_for_macos.md': { title: 'Building for macOS', position: 5 },
            'building_for_meta_quest.md': { title: 'Building for Meta Quest', position: 6 },
            'building_for_windows.md': { title: 'Building for Windows', position: 7 },
          };

          const platform = platformMap[filename];
          if (platform) {
            const githubUrl = `https://github.com/facebookresearch/ocean/blob/main/${filename}`;
            const tip = `\n\n:::tip\nThis documentation is also available on [GitHub](${githubUrl}).\n:::\n`;

            return {
              filename: filename,
              content: `---
title: ${platform.title}
sidebar_position: ${platform.position}
description: ${platform.title}
---

${modifiedContent}${tip}`,
            };
          }
          return undefined;
        },
      },
    ],
  ],

  presets: [
    [
      require.resolve('docusaurus-plugin-internaldocs-fb/docusaurus-preset'),
      /** @type {import('docusaurus-plugin-internaldocs-fb').PresetOptions} */
      ({
        docs: {
          sidebarPath: require.resolve('./sidebars.js'),
          editUrl: fbContent({
            internal: 'https://www.internalfb.com/intern/diffusion/FBS/browse/master/xplat/ocean/website/',
            external: 'https://github.com/facebookresearch/ocean/edit/main/website',
          }),
        },
        staticDocsProject: 'ocean',
        trackingFile: 'fbcode/staticdocs/WATCHED_FILES',
        theme: {
          customCss: ['./src/css/custom.css'],
        },
        'remark-code-snippets': {
          baseDir: '..',
        },
        internSearch: true,
        enableEditor: false,
        gtag: {
          trackingID: 'G-2MN2KM383Z',
          anonymizeIP: true,
        },
      }),
    ],
  ],

  themeConfig:
    /** @type {import('@docusaurus/preset-classic').ThemeConfig} */
    ({
      navbar: {
        title: 'Ocean',
        logo: {
          alt: 'Ocean Logo',
          src: 'img/ocean_logo_circular.svg',
        },
        items: [
          {
            type: 'doc',
            docId: 'introduction',
            position: 'left',
            label: 'Documentation',
          },
          {
            href: 'pathname:///doxygen/index.html',
            position: 'left',
            label: 'Reference',
          },
          {
            href: 'https://github.com/facebookresearch/ocean',
            label: 'GitHub',
            position: 'right',
          },
        ],
      },
      footer: {
        style: 'dark',
        links: [
          {
            title: 'Documentation',
            items: [
              {
                label: 'Introduction',
                to: '/docs/introduction',
              },
              {
                label: 'Building',
                to: '/docs/building',
              },
            ],
          },
          {
            title: 'Legal',
            // Please do not remove the privacy and terms, it's a legal requirement.
            items: [
              {
                  label: 'Privacy',
                  href: 'https://opensource.facebook.com/legal/privacy/',
              },
              {
                  label: 'Terms',
                  href: 'https://opensource.facebook.com/legal/terms/',
              },
            ],
          },
          {
            title: 'Legal (continued)',
            // Please do not remove the privacy and terms, it's a legal requirement.
            items: [
              {
                  label: 'Data Policy',
                  href: 'https://opensource.facebook.com/legal/data-policy/',
              },
              {
                  label: 'Cookie Policy',
                  href: 'https://opensource.facebook.com/legal/cookie-policy/',
              },
            ],
          },
        ],
        copyright: `Copyright © ${new Date().getFullYear()} Meta Platforms, Inc. Built with Docusaurus.`,
      },
      prism: {
        theme: prismThemes.github,
        darkTheme: prismThemes.dracula,
        additionalLanguages: ['bash', 'batch', 'powershell'],
      }
    }),
});
