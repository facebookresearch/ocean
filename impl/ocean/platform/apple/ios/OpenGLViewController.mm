/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/ios/OpenGLViewController.h"

#import <OpenGLES/ES2/glext.h>

#import <Foundation/Foundation.h>

#import <UIKit/UIKit.h>

@interface OpenGLViewController ()
{
}

/// The GLES context which is used
@property (strong, nonatomic) EAGLContext* context;

/**
 * Setup function for OpenGL ES.
 */
- (void)setupGL;

/**
 * Teardown function for OpenGL ES.
 */
- (void)tearDownGL;

@end

@implementation OpenGLViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

	if (!self.context)
	{
		NSLog(@"Failed to create ES context");
	}

	GLKView *view = (GLKView *)self.view;
	view.context = self.context;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

	[self setupGL];
}

- (void)makeOpenGLContextCurrent
{
	const bool result = [EAGLContext setCurrentContext:self.context];

	ocean_assert_and_suppress_unused(result, result);
}

- (void)dealloc
{
	[self tearDownGL];

	if ([EAGLContext currentContext] == self.context)
	{
		[EAGLContext setCurrentContext:nil];
	}
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];

	if ([self isViewLoaded] && ([[self view] window] == nil))
	{
		self.view = nil;

		[self tearDownGL];

		if ([EAGLContext currentContext] == self.context)
		{
			[EAGLContext setCurrentContext:nil];
		}

		self.context = nil;
	}

	// Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden
{
	return YES;
}

- (void)setupGL
{
	[EAGLContext setCurrentContext:self.context];
}

- (void)tearDownGL
{
	[EAGLContext setCurrentContext:self.context];
}

@end
