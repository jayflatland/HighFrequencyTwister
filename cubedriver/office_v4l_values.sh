#!/bin/sh

v4l2-ctl -d /dev/v4l/by-id/usb-046d_081b_20648CC0-video-index0 --list-ctrls
echo
v4l2-ctl -d /dev/v4l/by-id/usb-046d_081b_B9548CC0-video-index0 --list-ctrls
echo
v4l2-ctl -d /dev/v4l/by-id/usb-046d_081b_B6A2CCC0-video-index0 --list-ctrls
echo
v4l2-ctl -d /dev/v4l/by-id/usb-046d_081b_866188E0-video-index0 --list-ctrls
echo
