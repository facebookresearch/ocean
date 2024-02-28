// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/navigation/DisplayMapsRenderingExperience.h"
#include "ocean/platform/meta/Login.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

#include <FBUserSession/FBUserSession.h>
#include <FBUserSession/FBAccessTokenManaging.h>

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY) && defined(__OBJC__)

#import <Mapbox/Mapbox.h>
#import <UIKit/UIKit.h>

@class  MapViewLayerManager;

@interface MapViewController : UIViewController <MGLMapViewDelegate>

- (instancetype)initWithLayerManager:(MapViewLayerManager*)layerManager;

@property (readonly, strong) MGLMapView* mapView;
@property (readonly, strong) MapViewLayerManager* layerManager;

- (void)updateMapViewCameraWithDictionary:(NSDictionary*)dictionary;

@end

@interface MapController : NSObject {
  Ocean::XRPlayground::DisplayMapsRenderingExperience *experience;
  const Ocean::Interaction::UserInterface* userInterface;
}

- (id)initWithCPPInstance:(Ocean::XRPlayground::DisplayMapsRenderingExperience*)cppInstance
            userInterface:(const Ocean::Interaction::UserInterface*)interface;

@property(strong) UIViewController* viewController;

@end

@interface MapViewLayerManager : NSObject {
  NSString* _accessToken;
}

@property (nonatomic, strong) NSString* styleID;
@property (nonatomic, strong) NSString* version;

- (instancetype)initWithToken:(NSString *)authToken;
- (void)updateLayerProperties:(NSDictionary*)properties;
- (NSURL*)styleURL;

@end

@interface NSURLComponents (QueryToDictionary)
- (NSDictionary*)queryItemsDictionary;
@end

@implementation NSURLComponents (QueryToDictionary)

- (NSDictionary*)queryItemsDictionary {
  NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];
  for (NSURLQueryItem* item in self.queryItems) {
    dict[item.name] = item.value;
  }

  return dict;
}

@end

@implementation MapController

- (id)initWithCPPInstance:(Ocean::XRPlayground::DisplayMapsRenderingExperience*)cppInstance
            userInterface:(const Ocean::Interaction::UserInterface*)interface {
  self = [super init];
  if (self) {
    experience = cppInstance;
    userInterface = interface;
    _viewController = interface->viewController();
  }

  return self;
}

@end

@implementation MapViewController

- (instancetype)initWithLayerManager:(MapViewLayerManager*)layerManager {
  self = [super init];
  if (self) {
    _layerManager = layerManager;
    _mapView = [[MGLMapView alloc] initWithFrame:CGRectZero];
  }
  return self;
}

- (void)viewDidLoad {
  [super viewDidLoad];

  [self.navigationItem setTitle:@"Display Maps Render"];
  UIBarButtonItem* menuButton = [[UIBarButtonItem alloc] initWithTitle:@"Menu" style:UIBarButtonItemStylePlain target:self action:@selector(showMenu)];
  [self.navigationItem setRightBarButtonItem:menuButton];

  if (@available(iOS 13.0, *)) {
    self.view.backgroundColor = [UIColor systemBackgroundColor];
  } else {
    self.view.backgroundColor = [UIColor whiteColor];
  }

  [_mapView setStyleURL:[self.layerManager styleURL]];
  _mapView.delegate = self;

  [_mapView setTranslatesAutoresizingMaskIntoConstraints:NO];
  [self.view addSubview:_mapView];

  [NSLayoutConstraint activateConstraints:@[
    [_mapView.topAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.topAnchor],
    [_mapView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor],
    [_mapView.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor],
    [_mapView.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor]
  ]];

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didRecieveDeepLinkNotification:) name:@"com.xrplayground.appDidOpenFromURL" object:nil];
}

- (void)updateMapViewCameraWithDictionary:(NSDictionary*)dictionary {

  MGLMapCamera* camera = [[MGLMapCamera alloc] init];

  if (dictionary[@"center[0]"] && dictionary[@"center[1]"]) {
    CLLocationCoordinate2D center = CLLocationCoordinate2DMake([dictionary[@"center[0]"] doubleValue], [dictionary[@"center[1]"] doubleValue]);
    camera.centerCoordinate = center;
  }

  if (dictionary[@"bearing"]) {
    camera.heading = [dictionary[@"bearing"] doubleValue];
  }

  if (dictionary[@"pitch"]) {
    camera.pitch = [dictionary[@"pitch"] floatValue];
  }

  [self.mapView setCamera:camera];

  if (dictionary[@"zoom"]) {
    [self.mapView setZoomLevel:[dictionary[@"zoom"] doubleValue]];
  }
}

- (void)showMenu {
  UIAlertController* menu = [UIAlertController alertControllerWithTitle:@"Menu" message:nil preferredStyle:UIAlertControllerStyleActionSheet];
  [menu addAction:[UIAlertAction actionWithTitle:@"Dismiss" style:UIAlertActionStyleCancel handler:nil]];

  if (UIPasteboard.generalPasteboard.hasURLs) {
    [menu addAction:[UIAlertAction actionWithTitle:@"Paste URL" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
      [self didTapPasteMenuItem];
    }]];
  }

  [self presentViewController:menu animated:YES completion:nil];
}

- (void)didTapPasteMenuItem {
  NSURL* pasteboardURL = UIPasteboard.generalPasteboard.URL;
  NSURLComponents* components = [NSURLComponents componentsWithURL:pasteboardURL resolvingAgainstBaseURL:NO];
  NSDictionary* properties = [components queryItemsDictionary];
  [self updateMapWithDeepLinkProperties:properties];
}

- (void)didRecieveDeepLinkNotification:(NSNotification*)notification {
  NSURLComponents* urlComponents = [notification userInfo][@"com.xrplayground.appDidOpenFromURL.URLComponentsKey"];
  NSDictionary* properties = [urlComponents queryItemsDictionary];
  [self updateMapWithDeepLinkProperties:properties];
}

- (void)updateMapWithDeepLinkProperties:(NSDictionary*)properties {
  [self.layerManager updateLayerProperties:properties];

  [_mapView setStyleURL:[self.layerManager styleURL]];
  [self updateMapViewCameraWithDictionary:properties];
}

- (void)dealloc {
  [[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end

@implementation MapViewLayerManager

- (instancetype)initWithToken:(NSString*)authToken {
  return [self initWithToken:authToken layerProperties:nil];
}

- (instancetype)initWithToken:(NSString*)authToken layerProperties:(NSDictionary*)properties {
  self = [super init];
  if (self) {
    _accessToken = authToken;
    _styleID = @"canterbury_1_0";
    [self updateLayerProperties:properties];
  }
  return self;
}

- (void)updateLayerProperties:(NSDictionary*)properties {
  if (properties[@"style"]) {
    _styleID = properties[@"style"];
  }
  if (properties[@"v"]) {
    _version = properties[@"v"];
  }
}

- (NSURL*)styleURL {
  NSString *baseURLString = [NSString stringWithFormat:@"https://www.facebook.com/maps/vt/style/%@/", self.styleID];
  NSURLComponents* urlComponents = [NSURLComponents componentsWithString:baseURLString];
  NSMutableArray* queryItems = [[NSMutableArray alloc] init];

  NSURLQueryItem* accessTokenItem = [NSURLQueryItem queryItemWithName:@"access_token" value:_accessToken];
  [queryItems addObject:accessTokenItem];

  if (self.version) {
    NSURLQueryItem* versionItem = [NSURLQueryItem queryItemWithName:@"v" value:self.version];
    [queryItems addObject:versionItem];
  }

  urlComponents.queryItems = queryItems;

  return [urlComponents URL];
}

@end

#endif // __OBJC__

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

namespace {
MapController* _mapController;
}

static NSString* getAuthToken() {
  auto session = Platform::Meta::Login::get().userSession(Platform::Meta::Login::LT_FACEBOOK);
  NSString* loggedInAccountID = session.loggedInAccountID;
  auto fbAccessToken = [[session accessTokenManagerForFBID:loggedInAccountID] accessTokenForFBID:loggedInAccountID];
  std::string tokenString = [fbAccessToken.token cStringUsingEncoding:1];
  return [NSString stringWithCString:tokenString.c_str()
                            encoding:[NSString defaultCStringEncoding]];
}

static NSURLComponents* componentsFromProperties(const std::string& properties) {
  NSString* propertiesString = [NSString stringWithCString:properties.c_str()
                                                  encoding:[NSString defaultCStringEncoding]];
  NSString* urlString = [NSString stringWithFormat:@"?%@", propertiesString]; // need valid url for parsing
  NSString* escaped = [urlString stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
  NSURLComponents* components = [NSURLComponents componentsWithString:escaped];
  return components;
}

void DisplayMapsRenderingExperience::setUpIOSInterface(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, DisplayMapsRenderingExperience& experience, const std::string& properties) {

  _mapController = [[MapController alloc] initWithCPPInstance:&experience userInterface:&userInterface];

  NSDictionary* props = [componentsFromProperties(properties) queryItemsDictionary];

  MapViewLayerManager* layerManager = [[MapViewLayerManager alloc] initWithToken:getAuthToken() layerProperties:props];
  MapViewController* mapViewController = [[MapViewController alloc] initWithLayerManager:layerManager];
  [mapViewController updateMapViewCameraWithDictionary:props];

  UINavigationController* navController = [[UINavigationController alloc] initWithRootViewController:mapViewController];
  if (@available(iOS 13.0, *)) {
    UINavigationBarAppearance* appearance = [[UINavigationBarAppearance alloc] init];
    [appearance configureWithDefaultBackground];
    navController.navigationBar.scrollEdgeAppearance = appearance;
    navController.navigationBar.standardAppearance = appearance;
    navController.navigationBar.compactAppearance = appearance;
  }

  [navController setModalPresentationStyle:UIModalPresentationFullScreen];
  [_mapController.viewController presentViewController:navController animated:YES completion:nil];
}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
