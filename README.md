macOS X Configuration Tool for 3Dconnexion CadMouse
-------------------------------------------

Since 3Dconnexion does not have any open-source tool to change firmware
settings for their CadMouse device, and doesn't really care about any OS
besides windows, I forked the awesome linux program [cadmousectl]
(https://github.com/lachs0r/cadmousectl) by Martin Herkt who took the time
to reverse engineer the entire protocol with a virtual machine.

Anyway, this is my humble attempt at an OS X port.

Compilation
===========

This has one dependency, hidapi.  Install it in /usr/local manually, or
use brew:

```bash
  /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  brew install hidapi
```

and you're done!

Building and using the OS X version of cadmousectl is as easy as:
```bash
cd <repo clone location>
make
./cadmousectl <options>
```

Usage
=====

cadmousectl [-[lprsS] value]

| Option | Effect                                                  |
|--------|---------------------------------------------------------|
| -l     | Enable (non-zero) or disable (zero) lift-off detection. |
| -p     | Set polling rate (125, 250, 500, 1000).                 |
| -r     | Remap buttons. Format is real_button:assigned_button.<br><br>Values for real_button:<br>left, right, middle, wheel, forward, backward, rm<br><br>Values for assigned_button:<br>left, right, middle, backward, forward, rm, extra   <br><br>*note:The extra button was discovered by accident. Using this, you can assign an additional button to the wheel click. It will have id 11 on X11*.         
| -s     | Set speed (1-164). <br>Cannot be used with -d                                                                                                                               
| -d     | Set speed in DPI (50-8200).<br> Cannot be used with -s                                  
| -S     | Set Smart Scroll mode. There are two additional modes <br> 0 = off, 1 = normal, 2 = slow, 3 = accelerated.  



License
=======

This software is available under the terms of the ISC license as it appears
in each source file.
