Tiny CLI tool to always switch default mic from AirPods and keep them balanced.

### Usage

`bin/airbud AIRPODS_NAME DEFAULTMIC_NAME` will start the application, which will then switch input device away from `AIRPODS_NAME` device to `DEFAULTMIC_NAME` and restore `AIRPODS_NAME`'s balance, and will repeat the same procedure for whenever input or output devices changes.

### Launchctl

Also there's **launchctl** agent for simplicity, one can edit `net.shdwp.airbud.plist` to include correct arguments:

 * `<string>Vasyl’s AirPods</string>` is the `AIRPODS_NAME` (take notice that it's a `’`, not `'`)
 * `<string>Built-in Microphone</string>` is the `DEFAULTMIC_NAME`

then copy/simlink the `bin/airbud` to `/opt/local/bin/airbud`, and move the `net.shdwp.airbud.plist` to `~/Library/LaunchAgents`. 

It now should start on system reboot, and manually loaded via `launchctl load ~/Library/LaunchAgents/net.shdwp.airbud.plist`.
