#import <Foundation/Foundation.h>
#import <AppKit/NSWorkspace.h>
#include <string>

namespace FlamingTorch
{
    const char *OSXPreferredStorageDirectory()
    {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        CFArrayRef a = (CFArrayRef) NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        CFRetain(a);
        [pool release];
        
        if (CFArrayGetCount(a) > 0)
        {
            CFStringRef s = (CFStringRef)CFArrayGetValueAtIndex(a, 0);
            
            std::string Path;
            Path.resize(PATH_MAX);
            CFStringGetFileSystemRepresentation(s, &Path[0], PATH_MAX);
            
            CFRelease(a);
            
            return Path.c_str();
        }
        
        CFRelease(a);
        
        return "";
    };
    
    const char *OSXResourcesDirectory()
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        
        std::string rpath;
        NSBundle* bundle = [NSBundle mainBundle];
        
        if (bundle != nil) {
            NSString* path = [bundle resourcePath];
            rpath = [path UTF8String] + std::string("/");
        }
        
        [pool drain];
        
        return rpath.c_str();
    };
};
