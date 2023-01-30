Reesist Core Latest (in development)
===========================

|CI|master|develop|
|-|-|-|

**About Reesist**
---------------

2022-2023 has shown us the lengths that certain entities will go through to control what they fear and or do not understand. Reesist will be a community driven and run project. There will be no owner, no VC backing, no controlling investors, no doxxed developers unless they choose to. There is nobody that can shut it down once the Genesis block is completed. 

We will Reesist!

Reesist is a Raptoreum (RTM) fork (thank you RTM), the history traced back is: Bitcoin > Dash (Darkcoin) > Raptoreum > Reesist.

**Why?**
-----

Raptoreums combination of Proof of Work (POW) and Smartnodes which offer an option for passive income and offering the network increased protection and decentralization is optimal to being able to Reesist.

We will Reesist!

**GPU over CPU**
--------------

Reesist will change the POW algorithm from Ghostrider to Ravencoins KAWPOW, both of these POW algorithms are ASIC and FPGA resistant. GPU these days are an excellent choice for securing the network and staying decentralized. Most laptops and desktops come with a capable GPU that can contribute to the network and be rewarded. They are more environmentally friendly than ASICs and available in far greater numbers.

We will Reesist!

**Smartnodes > Resistors**
------------------------

Due to the importance and services provided by Smartnodes their naming will be changed to "Resistors".

We will Reesist!

**Join Me and Wait For Mainnet**
------------------------------

I will not use centralized chats such as Discord or Telegram. I invite you to join me on Matrix based chat, Element is a sleek and easy way to join and participate on a decentralized fulle e2E encrypted chat (Matrix). Element has free open-source apps for browser, Desktop, Mac and mobile.
**Invite Link:** https://matrix.to/#/#reesist:envs.net

**Twitter:** https://twitter.com/Reesist_RES

You can also contact me at: therottenraven@proton.me

We will Reesist!

**Mainnet Launch**
----------------

Mainnet is approximately one month away. It will be a fair launch, there will be no ICO. There will be a small premine and ongoing developer fee.

**Premine:** Amount is still being discussed, but it will not be large. Premine is needed to ensure enough Resistors can be put up early on to activate Quorums and gain the additional network protections (chainlocks) such as 51% protection.

**Developer Fee:** A small developer fee is taken out of each block reward, this is used to pay for development and marketing.

We will Reesist!

**Exchanges**
-----------

Centralized exchanges should not be pursued for listings, these are a long-term threat to any Crypto project. Decentralized exchanges such as Atomic Dex are a better and safer choice!

We will Reesist!

License
-------

Reesist Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is meant to be stable. Development is done in separate branches.
[Tags](https://github.com/Reesist/reesist/tags) are created to indicate new official,
stable release versions of Reesist Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/test) are installed) with: `test/functional/test_runner.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and OS X, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.
