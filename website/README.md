# Website

This website is built using [Docusaurus 2](https://docusaurus.io/), a modern static website generator.

### Prerequisites

The following software packages will be required to test the website locally:

* [Node.js](https://nodejs.org/en/download)
* The `yarn` package manager

Mac users can either install Node.js

* manually and enable `yarn` by typing `corepack enable` in terminal, or
* using a package manager like [Homebrew](https://brew.sh/): `brew install yarn`

Users of other operating systems can hopefully extrapolated the necessary steps from the information above.

### Local Development

When you run this for the first time, a myriad of packages needs to be installed. To do this, run following from inside the `website/` directory:

```
$ yarn
```

or

```
$ yarn install
```

After that, start a local webserver by running (again from inside `website/`):

```
$ yarn start
```

This command starts a local development server and opens up a browser window. Most changes are reflected live without having to restart the server.

### Build

```
$ yarn build
```

This command generates static content into the `build` directory and can be served using any static contents hosting service.
