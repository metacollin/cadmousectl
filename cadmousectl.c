/* Copyright © 2015, Martin Herkt <lachs0r@srsfckn.biz>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or  without fee is hereby granted,  provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL,  DIRECT,  INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING  FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "hidapi.h"


#define _NULL_ 0

#define CADMOUSE_VID 0x256f
#define CADMOUSE_PID 0xc650


int cadmouse_send_command(hid_device *device, int opt, int val1, int val2)
{
    unsigned char cmd[8] = { 0x0c, opt, val1, val2, 0x00, 0x00, 0x00, 0x00 };
    int result = hid_send_feature_report(device, cmd, 8);

    return result < 0 ? result : 0;
}

int cadmouse_set_smartscroll(hid_device *device, int state)
{
    int result;

    if (state == 1 || state == 3)
        result = cadmouse_send_command(device, 0x03, 0x00, 0x00);
    else
        result = cadmouse_send_command(device, 0x03, 0x00, 0x01);

    if (result < 0)
        return result;

    if (state == 3)
        result = cadmouse_send_command(device, 0x04, 0xff, 0x00);
    else
        result = cadmouse_send_command(device, 0x04, state ? 0x00 : 0xff, 0x00);

    if (result < 0)
        return result;

    result = cadmouse_send_command(device, 0x05, 0x00, state ? 0x01 : 0x00);

    return result;
}

enum cadmouse_pollrate {
    POLL_125 = 0x08,
    POLL_250 = 0x04,
    POLL_500 = 0x02,
    POLL_1000 = 0x01
};

int cadmouse_set_pollrate(hid_device *device, enum cadmouse_pollrate rate)
{
    return cadmouse_send_command(device, 0x06, 0x00, rate);
}

int cadmouse_set_liftoff_detection(hid_device *device, int state)
{
    return cadmouse_send_command(device, 0x07, 0x00, state ? 0x00 : 0x1f);
}

typedef struct Button {
    char *name;
    char id;
} Button;

Button HWButtons[] = {
    { "left", 0x0a },
    { "right", 0x0b },
    { "middle", 0x0c },
    { "wheel", 0x0d },
    { "forward", 0x0e },
    { "backward", 0x0f },
    { "rm", 0x10 },
    { _NULL_, _NULL_ }
};

Button SWButtons[] = {
    { "left", 0x0a },
    { "right", 0x0b },
    { "middle", 0x0c },
    { "backward", 0x0d },
    { "forward", 0x0e },
    { "rm", 0x2e },
    { "extra", 0x2f },
    { _NULL_, _NULL_ }
};

Button *get_button(const char *name, Button *type)
{
    for (; type->name != _NULL_; type++) {
        if (strcasecmp(type->name, name) == 0)
            break;
    }

    return type->name != _NULL_ ? type : _NULL_;
}

int cadmouse_set_hwbutton(hid_device *device, Button *hw, Button *sw)
{
    return cadmouse_send_command(device, hw->id, 0x00, sw->id);
}

int cadmouse_set_speed(hid_device *device, int speed)
{
    return cadmouse_send_command(device, 0x01, 0x00, speed);
}


void print_help(void)
{
  printf("\n\nUsage:\n");
  printf("      \033[32;1m -l \033[0m Enable (non-zero) or disable (zero) lift-off detection.\n");
  printf("      \033[32;1m -p \033[0m 125, 250, 500, or 1000 Set polling rate (in hertz)\n");
  printf("      \033[32;1m -r \033[0m Remap buttons. Format is real_button:assigned_button.\n");
  printf("             real_button:     left, right, middle, wheel, forward, backward, rm\n");
  printf("             assigned_button: left, right, middle, backward, forward, rm, extra\n");
  printf("      \033[32;1m -s \033[0m Set speed (1-164).  Cannot be used with -d)\n");
  printf("      \033[32;1m -d \033[0m SSet speed in DPI (50-8200).  Cannot be used with -s)\n");
  printf("      \033[32;1m -S \033[0m Set Smart Scroll mode\n");
  printf("             modes: 0 is off, 1 is normal, 2 is slow, 3 is accelerated scrolling\n");
}


#define COMMAND(cmd, ...)           \
    do {                            \
        res = cmd(handle, __VA_ARGS__); \
        if (res) {                  \
            perror(#cmd);           \
            goto error;             \
        }                           \
    } while (0)

int main(int argc, char **argv)
{
    int opt, res;
    int mouseFound = 0;
    hid_device *handle;

    // Enumerate and print the HID devices on the system
    struct hid_device_info *devs, *cur_dev;
    
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs; 
    while (cur_dev) {

      if ((cur_dev->vendor_id == CADMOUSE_VID) & (cur_dev->product_id == CADMOUSE_PID))
      {
        mouseFound = 1; 
      printf("╠══╦══CadMouse device found!═══╣\n   ║ ╔══════════╗ ╔══════════╗\n   ╠━╣VID: %04hx ╠═╣PID: %04hx ║\n   ║ ╚══════════╝ ╚══════════╝\n   ╩", cur_dev->vendor_id, cur_dev->product_id);
      printf("\n");
      }
    
      
      cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    if (mouseFound == 0)
    {
      printf("Ruh roh.  I can't seem to find a CadMouse on your USB bus.  Make sure the cat didn't pull the cable out again.\n");
      goto error;
    }

    handle = hid_open(CADMOUSE_VID, CADMOUSE_PID, _NULL_);
    if (handle == _NULL_)
    {
      printf("Oh noes! I found the mouse, but I can't get a handle on it.\n");
      goto error;
    }

      int validcommand = 0;

      extern char *optarg;
      extern int optind, opterr, optopt;
      int speedconflict = 0;
      while ((opt = getopt(argc, argv, "l:p:r:s:d:S:")) != -1) {
          switch(opt) {
              case 'l':
                  {
                      validcommand = 1;
                      long int liftdetect = strtol(optarg, _NULL_, 10);

                      if (liftdetect == 0)
                          COMMAND(cadmouse_set_liftoff_detection, 0);
                      else
                          COMMAND(cadmouse_set_liftoff_detection, 1);
                  }
                  break;
              case 'p':
                  {
                      validcommand = 1;
                      long int rate = strtol(optarg, _NULL_, 10);

                      if (rate == 125)
                          COMMAND(cadmouse_set_pollrate, POLL_125);
                      else if (rate == 250)
                          COMMAND(cadmouse_set_pollrate, POLL_250);
                      else if (rate == 500)
                          COMMAND(cadmouse_set_pollrate, POLL_500);
                      else if (rate == 1000)
                          COMMAND(cadmouse_set_pollrate, POLL_1000);
                      else
                          fputs("-p: Unsupported polling rate\n", stderr);
                  }
                  break;
              case 'r':
                  {
                      validcommand = 1;
                      Button *hw = _NULL_, *sw = _NULL_;
                      char *sep = strchr(optarg, ':');

                      if (sep != _NULL_) {
                          *sep = '\0';
                          sep++;
                          hw = get_button(optarg, HWButtons);
                          sw = get_button(sep, SWButtons);
                      }

                      if (hw == _NULL_ || sw == _NULL_)
                          fputs("-r: invalid button mapping\n", stderr);
                      else
                          COMMAND(cadmouse_set_hwbutton, hw, sw);
                  }
                  break;
              case 's':
                  {
                      validcommand = 1;
                      long int speed = strtol(optarg, _NULL_, 10);
                      if (speedconflict != 0)
                          fputs("-s: -s cannot be used with -d or used more than once\n", stderr);
  		    else
                      if (speed < 1 || speed > 164)
                          fputs("-s: Option value out of range\n", stderr);
                      else
                          speedconflict++;
                          COMMAND(cadmouse_set_speed, speed);
                  }
                  break;
               case 'd':
                  {
                      validcommand = 1;
                      long int dpi = strtol(optarg, _NULL_, 10);
                      long int speed = (dpi * 164)/8200;
                      if (speedconflict != 0)
                          fputs("-d: -d cannot be used with -s or used more than once\n", stderr);
                      if (dpi < 50 || dpi > 8200)
                          fputs("-d: Option value out of range\n", stderr);
                      else
  			                  speedconflict++;
                          COMMAND(cadmouse_set_speed, speed);
                  }       
                  break;
              case 'S':
                  {
                      validcommand = 1;
                      long int smartscroll = strtol(optarg, _NULL_, 10);

                      if (smartscroll >= 0 && smartscroll < 4)
                          COMMAND(cadmouse_set_smartscroll, smartscroll);
                      else
                          fputs("-S: Option value out of range\n", stderr);
                  }
                  break;
          }
      }

      if (validcommand == 0)
      {
        print_help();
      }

  error:
      return -1;
  }

