# host_cdc_msc_hid
Code for Pico Computer

1. Create directory /Users/andrew/pico

2. cd /Users/andrew/pico

3. git clone https://github.com/Wren6991/PicoDVI.git

4. git clone https://github.com/raspberrypi/pico-sdk.git

5. git clone https://github.com/raspberrypi/pico-examples.git

6. cd /Users/andrew/pico/pico-sdk

7. git submodule update --init

8. cd /Users/andrew/pico/pico-examples/usb/host/

9. rm -r host_cdc_msc_hid

10. git clone https://github.com/andrewbayly/host_cdc_msc_hid.git

11. export PICO_SDK_PATH="/Users/andrew/pico/pico-sdk"

12. cd /Users/andrew/pico/pico-examples

13. mkdir build

14. cd build

15. cmake ..

16. cd /Users/andrew/pico/pico-examples/build/usb/host/host_cdc_msc_hid

17. make

18. BootSel the Pico

19. Copy host_cdc_msc_hid.uf2 from /Users/andrew/pico/pico-examples/build/usb/host/host_cdc_msc_hid onto the Pico
