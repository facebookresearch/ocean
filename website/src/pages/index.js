/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import React from 'react';
import clsx from 'clsx';
import Layout from '@theme/Layout';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import useBaseUrl from '@docusaurus/useBaseUrl';
import styles from './styles.module.css';
import {useColorMode} from '@docusaurus/theme-common';

const features = [
  {
    title: 'Features',
    description: (
      <>
        Image Processing<div/>
        Photogrametry & Computer Vision algorithms<div/>
		    Visualization & Rendering<div/>
      </>
    ),
  },
  {
    title: 'Multi-platform',
    description: (
      <>
        iOS/Android/Quest<div/>
		    Windows/macOS/Linux<div/>
      </>
    ),
  },
  {
    title: 'Usability',
    description: (
      <>
        High performance<div/>
        Low binary impact<div/>
        Detailed documentation and examples<div/>
      </>
    ),
  },
];

function Feature({imageUrl, title, description}) {
  const imgUrl = useBaseUrl(imageUrl);
  return (
    <div className={clsx('col col--4', styles.feature)}>
      {imgUrl && (
        <div className="text--center">
          <img className={styles.featureImage} src={imgUrl} alt={title} />
        </div>
      )}
      <h3>{title}</h3>
      <p>{description}</p>
    </div>
  );
}

function LogoImage() {
  const {colorMode} = useColorMode();
  const logoBlackText = useBaseUrl('img/ocean_logo_full_dark-theme.svg');
  const logoWhiteText = useBaseUrl('img/ocean_logo_full_light-theme.svg');
  return (
    <img
      className={styles.oceanBanner}
      src={colorMode === 'dark' ? logoBlackText : logoWhiteText}
      alt="Ocean Logo"
      width="800"
    />
  );
}

export default function Home() {
  const context = useDocusaurusContext();
  const {siteConfig = {}} = context;
  return (
    <Layout>
      <header className={clsx('hero hero--primary', styles.oceanBanner)}>
        <div className="container">
          <LogoImage />
          <p className="hero__subtitle">{siteConfig.tagline}</p>
          <div className={styles.buttons}>
            <Link
              className={clsx(
                'button button--outline button--secondary button--lg',
                styles.getStarted,
              )}
              to={useBaseUrl('docs/introduction')}>
              Get Started
            </Link>
          </div>
        </div>
      </header>
      <main>
        {features && features.length > 0 && (
          <section className={styles.features}>
            <div className="container">
              <div className="row">
                {features.map(({title, imageUrl, description}) => (
                  <Feature
                    key={title}
                    title={title}
                    imageUrl={imageUrl}
                    description={description}
                  />
                ))}
              </div>
            </div>
          </section>
        )}
      </main>
    </Layout>
  );
}
