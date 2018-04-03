//
//  main.c
//  airbud
//
//  Created by Vasyl Horbachenko on 4/3/18.
//  Copyright © 2018 Vasyl Horbachenko. All rights reserved.
//

#include <stdio.h>
#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>
#include "objc/runtime.h"

char *TARGET_DEVICE;
char *FALLBACK_DEVICE;

void device_name(AudioDeviceID device, char *name) {
    UInt32 nameSize = 256;
    AudioDeviceGetProperty(device, 0, 0, kAudioDevicePropertyDeviceName, &nameSize, name);
}

bool cmp_device_name(AudioDeviceID device, char *target_name) {
    char name[256];
    device_name(device, name);
    
    return strcmp(target_name, name) == 0;
}

AudioDeviceID current_device(AudioHardwarePropertyID id) {
    AudioDeviceID device;
    UInt32 propertySize = sizeof(device);
    AudioHardwareGetProperty(id, &propertySize, &device);
    return device;
}

AudioDeviceID find_device_by_name(char *required_name) {
    UInt32 size;
    AudioDeviceID devices[64];
    
    AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &size, 0);
    AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &size, devices);
    
    for (int i = 0; i < size / sizeof(AudioDeviceID); i++) {
        if (cmp_device_name(devices[i], required_name)) {
            return devices[i];
        }
    }
    
    return 0;
}

void restore_balance(AudioDeviceID device) {
    UInt32 size;
    Float32 leftVolume, rightVolume;

    AudioDeviceGetProperty(device, 1, false, kAudioDevicePropertyVolumeScalar, &size, &leftVolume);
    AudioDeviceGetProperty(device, 2, false, kAudioDevicePropertyVolumeScalar, &size, &rightVolume);
    
    Float32 volume = fmaxf(leftVolume, rightVolume);
    AudioDeviceSetProperty(device, NULL, 1, false, kAudioDevicePropertyVolumeScalar, sizeof(volume), &volume);
    AudioDeviceSetProperty(device, NULL, 2, false, kAudioDevicePropertyVolumeScalar, sizeof(volume), &volume);
    
    printf("Restored balance for output\n");
}

OSStatus input_change_tap(AudioHardwarePropertyID id, void *context) {
    AudioDeviceID device = current_device(kAudioHardwarePropertyDefaultInputDevice);

    if (cmp_device_name(device, TARGET_DEVICE)) {
        AudioDeviceID default_device = find_device_by_name(FALLBACK_DEVICE);
        AudioHardwareSetProperty(kAudioHardwarePropertyDefaultInputDevice, sizeof(AudioDeviceID), &default_device);
        printf("Switched input to fallback\n");
    }
    
    return 0;
}

OSStatus output_change_tap(AudioHardwarePropertyID id, void *context) {
    AudioDeviceID device = find_device_by_name(TARGET_DEVICE);
    
    printf("did\n");
    if (device) {
        restore_balance(device);
    }

    return 0;
}

int main(int argc, const char * argv[]) {
    if (argc != 3) {
        printf("Application requires 2 arguments!\n");
        return 1;
    }
    
    TARGET_DEVICE = argv[1];
    FALLBACK_DEVICE = argv[2];
    
    AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultInputDevice, &input_change_tap, NULL);
    AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultOutputDevice, &output_change_tap, NULL);
    
    output_change_tap(NULL, NULL);
    input_change_tap(NULL, NULL);

    pause();
}
