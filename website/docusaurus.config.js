/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Force staticdocs rebuild #1

const {fbContent, fbInternalOnly} = require('docusaurus-plugin-internaldocs-fb/internal');
const repoUrl = 'https://github.com/facebookresearch/ocean';

const lightCodeTheme = require('prism-react-renderer/themes/github');
const darkCodeTheme = require('prism-react-renderer/themes/dracula');

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
        experimentalXRepoSnippets: {
          baseDir: '.',
        },
        staticDocsProject: 'ocean',
        trackingFile: 'fbcode/staticdocs/WATCHED_FILES',
        blog: {
          showReadingTime: true,
          editUrl: 'https://www.internalfb.com/code/fbsource/xplat/ocean/website', // TODO change to path to your project in fbsource/www
        },
        theme: {
          customCss: require.resolve('./src/css/custom.css'),
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
          src: 'img/logo.svg',
        },
        items: [
          {
            type: 'doc',
            docId: 'intro',
            position: 'left',
            label: 'Tutorial',
          },
          {to: '/blog', label: 'Blog', position: 'left'},
          {
            href: 'https://github.com/facebook/docusaurus',
            label: 'GitHub',
            position: 'right',
          },
        ],
      },
      footer: {
        style: 'dark',
        links: [
          {
            title: 'Docs',
            items: [
              {
                label: 'Tutorial',
                to: '/docs/intro',
              },
            ],
          },
          {
            title: 'Community',
            items: [
              {
                label: 'Stack Overflow',
                href: 'https://stackoverflow.com/questions/tagged/docusaurus',
              },
              {
                label: 'Discord',
                href: 'https://discordapp.com/invite/docusaurus',
              },
              {
                label: 'Twitter',
                href: 'https://twitter.com/docusaurus',
              },
            ],
          },
          {
            title: 'More',
            items: [
              {
                label: 'Blog',
                to: '/blog',
              },
              {
                label: 'GitHub',
                href: 'https://github.com/facebook/docusaurus',
              },
            ],
          },
        ],
        copyright: `Copyright © ${new Date().getFullYear()} My Project, Inc. Built with Docusaurus.`,
      },
      prism: {
        theme: lightCodeTheme,
        darkTheme: darkCodeTheme,
      },
    }),
});
