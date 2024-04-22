# dont_trust

```bash
  ;                    
  ED.                  
  E#Wi                 
  E###G.               
  E#fD#W;     GEEEEEEEL
  E#t t##L    ,;;L#K;;.
  E#t  .E#K,     t#E   
  E#t    j##f    t#E   
  E#t    :E#K:   t#E   
  E#t   t##L     t#E   
  E#t .D#W;      t#E   
  E#tiW#G.       t#E   
  E#K##i         t#E   
  E##D.           fE   
  E#t              :   
  L:                   
```
<img src='./preview.gif'>

###### Each time a different onion service is created, that being the case, if trying to connect right after the creation, a minute or two may be needed given the fact the onion service may still be having a proper integration to the tor network

Secure peer to peer communication, opsec focused

## Table of contents
* [Development environment](#development-environment)
* [Building](#building)
* [Usage](#usage)
* [Protocol Specification](#protocol-specification)
* [Customization](#customization)

## Development environment
compile_flags.txt is for [usage with clangd](https://clangd.llvm.org/design/compile-commands#compilation-databases)<br>
The c standard being used is a dialect from c17, that being gnu17

## Building
### Dependencies
- gcc
- [libcrypto3](https://www.openssl.org/docs/man3.0/man7/crypto.html) (From openssl3) header files
- make (Or run the build command from the Makefile directly)
#### Fedora 39
```bash
sudo dnf install openssl-devel
```
#### Debian 12
```bash
sudo apt-get update
sudo apt-get install libssl-dev
```

To build:
```bash
$ make build
```
## Usage
### Dependencies
- An [ANSI escape sequences](https://en.wikipedia.org/wiki/ANSI_escape_code) conformant terminal with E3 capability (Most of the terminals nowadays, you probably should not need to worry about it if you aren't using some exotic setup)
- Tor, [the central project, providing the core software for using and participating in the Tor network, not the browser](https://gitlab.torproject.org/tpo/core/tor/) (If you intend to establish connections over it)
- [libcrypto3](https://www.openssl.org/docs/man3.0/man7/crypto.html) from openssl3
#### Fedora 39
```bash
sudo dnf install tor
sudo dnf install openssl-libs
```
#### Debian 12
```bash
sudo apt-get update
sudo apt-get install tor
sudo apt-get install libssl3
```

##### You must provide your rsa private key and the rsa public key from your peer:
```bash
$ dont_trust --pKey "./client_private_key_path.txt" --pubKey "./peer_public_key_path.pem"
```
###### The file type does not matter but the keys must be [PEM encoded](https://www.rfc-editor.org/rfc/rfc7468)
###### The possible sizes are those supported by [libcrypto](https://www.openssl.org/docs/man3.0/man7/crypto.html) from [openssl3](https://www.openssl.org/)

### Connecting
##### To connect to an ipv4 address:
```bash
$ dont_trust -t 192.168.0.1
```
###### The address must be human readable
##### To connect to an onion address:
```bash
$ dont_trust -o vww6ybal4bd7szmgncyruucpgfkqahzddi37ktceo3ah7ngmcopnpyyd.onion
```

### Listening for connections
##### To listen for normal ones:
```bash
$ dont_trust ...
```
###### Just dont use any connection option, that being "-t" and "-o"
##### To listen for onion ones:
Before that, do this first time setup:<br>
1 - Generate a hashed password for control port access, <your_password> length must not be bigger than 1000:
```bash
$ tor --hash-password <your_password>
```
2 - On your ["torrc" file](https://support.torproject.org/tbb/tbb-editing-torrc/), add the following:
```bash
ControlPort 9051
HashedControlPassword <The result from the above command>
```
3 - Reload the configs if tor is running:
```bash
$ sudo kill -s SIGHUP <tor process id>
```
<br>

Just provide your password
```bash
$ dont_trust --torControlPassword <your_password>
```

## Protocol Specification

Runs on top of tcp, there are 2 commands available:
###### A command is considered a network byte order 8 bits unsigned integer sent over the tcp byte stream

### 0x01 = Starts a message
After which should be on the following order:<br>

- An unsigned 32 bit number in network byte order, that's the message code
- An unsigned 32 bit number in network byte order containing the cipher text size in bytes
- The initialization vector used to decrypt the cipher text, a sequence of 16 bytes
- The private key encrypted session key (AES in this case) used to decrypt the ciphertext, a sequence of 256 bytes
- The cipher text, up to a maximum of 65552 bytes length

### 0x02 = Starts a message confirmation
After which should be on the following order:<br>

- An unsigned 32 bit number in network byte order, that's the number of confirmations codes
- n unsigned 32 bit numbers in network byte order, each being a message code from a particular message (Only your own messages have their code saved in the application, that's implementation dependent)

## Customization

### Changing button for application mode change:<br>
There are 2, "VIEW" and "EDIT", the default button for changing is the 0x1B ASCII code<br>
Update "STATUS_CHANGE_BUTTON" at "src/common/common.h" to a desired ASCII code<br>

### Changing buttons for line moving:<br>
If the program is on "VIEW" mode, bytes coming from stdin are used to go up or down<br>
Update "UP_BUTTON" or "DOWN_BUTTON" at "src/common/common.h" to a desired ASCII code, the defaults are 'k' for "UP_BUTTON" and 'j' for "DOWN_BUTTON"<br><br>

###### For the following color reletad changes, refer to [some terminal 256 color ids table](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#256-colors)
### Changing background color:
Update "TERMINAL_BACKGROUND_COLOR_ID" from "src/common/common.h" to a desired terminal color id<br>

### Changing status line color:
Update "STATUS_LINE_TERMINAL_COLOR_CODE" from "src/common/common.h" to a desired terminal color id<br>
The default is "27" (Some blue tone)

### Changing sent messages color:
##### Not yet received by peer
Update the value of "PEER_NOT_READ_TERMINAL_BACKGROUND_COLOR_ID" from "src/common/common.h"<br>
The default is "196" (Some red tone)
##### Already received by peer
Update the value of "PEER_READ_TERMINAL_BACKGROUND_COLOR_ID" from "src/common/common.h"<br>
The default is "41" (Some green tone)

### Changing received messages color:
Update the value of "RECEIVED_TERMINAL_BACKGROUND_COLOR_ID" from "src/common/common.h"<br>
The default is "232" (Black)
