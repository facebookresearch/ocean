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
