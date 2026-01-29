/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/ios/GLViewController.h"

#import <OpenGLES/ES2/glext.h>

#import <Foundation/Foundation.h>

#import <UIKit/UIKit.h>

@interface GLViewController ()
{
}

/// The GLES context which is used
@property (strong, nonatomic) EAGLContext* context_;

/**
 * Setup function for OpenGL ES.
 */
- (void)setupGL;

/**
 * Teardown function for OpenGL ES.
 */
- (void)tearDownGL;

@end

@implementation GLViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	self.context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

	if (!self.context_)
	{
		NSLog(@"Failed to create ES context");
	}

	GLKView *view = (GLKView *)self.view;
	view.context = self.context_;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

	[self setupGL];
}

- (void)makeOpenGLContextCurrent
{
	const bool result = [EAGLContext setCurrentContext:self.context_];

	ocean_assert_and_suppress_unused(result, result);
}

- (void)dealloc
{
	[self tearDownGL];

	if ([EAGLContext currentContext] == self.context_)
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

		if ([EAGLContext currentContext] == self.context_)
		{
			[EAGLContext setCurrentContext:nil];
		}

		self.context_ = nil;
	}

	// Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden
{
	return YES;
}

- (void)setupGL
{
	[EAGLContext setCurrentContext:self.context_];
}

- (void)tearDownGL
{
	[EAGLContext setCurrentContext:self.context_];
}

@end
