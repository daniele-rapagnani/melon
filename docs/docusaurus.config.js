module.exports = {
  title: 'melon',
  tagline: 'A modern embeddable programming language',
  url: 'https://your-docusaurus-test-site.com',
  baseUrl: '/',
  onBrokenLinks: 'throw',
  favicon: 'img/favicon.ico',
  organizationName: 'daniele-rapagnani', // Usually your GitHub org/user name.
  projectName: 'melon', // Usually your repo name.
  themeConfig: {
    navbar: {
      title: 'melon',
      logo: {
        alt: 'Melon Programming Language',
        src: 'img/melon_logo_symbol.png',
      },
      items: [
        {
          to: 'docs/',
          activeBasePath: 'docs',
          label: 'Docs',
          position: 'left',
        }
      ],
    },
    footer: {
      style: 'dark',
      links: [
        {
          title: 'Community',
          items: [
            {
              label: 'GitHub',
              href: 'https://github.com/daniele-rapagnani/melon',
            },
            {
              label: 'Stack Overflow',
              href: 'https://stackoverflow.com/questions/tagged/melon',
            },
          ],
        },
      ],
      copyright: `Copyright © ${new Date().getFullYear()} Daniele Rapagnani.`,
    },
  },
  presets: [
    [
      '@docusaurus/preset-classic',
      {
        docs: {
          sidebarPath: require.resolve('./sidebars.js'),
          // Please change this to your repo.
          editUrl:
            'https://github.com/daniele-rapagnani/edit/master/docs/',
        },
        theme: {
          customCss: require.resolve('./src/css/custom.css'),
        },
      },
    ],
  ],
};
