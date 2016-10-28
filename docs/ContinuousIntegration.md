How we (pre-)release usable MD380 Experimental Firmware
====

### TLDR
To provide tested, usable, experimental firmware initiated by [Travis](https://www.qrz.com/db/KK4VCZ) for the Tytera MD380 DMR handheld radio we've created a small CI (Continous Integration) system. This system picks up authored changes to the experimental firmware code and builds pre-releases of firmware. Together with a note of changes and accompanying CSV-files, containing ID's and names of HAMs, this is published at a second Github location.
Our release master, [PA3MET](https://www.qrz.com/db/PA3MET), ensures field-tests of the firmware have been completed and than releases the firmware to [this Github-page](https://github.com/roelandjansen/md380tools/releases/latest). While still **experimental**, HAMs can relatively safe install and test new features added by the developers. ***Installing the firmware is at your own risk***

### Workflow diagram

![Workflow diagram](JenkinsBuilder.png)

### Our challenge
Creating lots of updates and features to [Travis'](https://www.qrz.com/db/KK4VCZ) code on [Github](https://github.com/travisgoodspeed/md380tools), amongst many others, [DF8AV](https://www.qrz.com/db/DF8AV), [PH7WIM](https://www.qrz.com/db/PH7WIM) & [PA9TV](https://www.qrz.com/db/PA9TV) produced 'wannahave' features for HAMs owning a MD380. There were however two distinct challenges. 

  * HAMs would need a machine to build the firmware from the code
  * HAMs would search for pre-build ones, unknowing the stability of the firmware
 
Within a group of HAMs we decided this should be dealt with so every HAM could - at her/his own risk - enjoy the experimental firmware features.

### Continuous Integration on Github
Github joines forces on CI with [Travis](http://travis-ci.org) (the tooling, not the author), and [Travis'](https://www.qrz.com/db/KK4VCZ) (the author) repository actually is run through it on commits. It is however not easy to create releases on the repositories release-page. Even then these should be pre-releases to create a safe indication for HAMs to distinguish 'nice-idea' firmware with potential flaws from the 'actually runs' firmware tested on a few radio's.
By example, as comitters, we own [PD0ZRY](https://www.qrz.com/db/PD0ZRY) for providing HAMs with timely builds on his website. Keeping an eye on the Github updates he frequently builds and releases firmware to his page. As indicated on his page, the available firmware should be handled-with care. It's just build, not validated/tested. This is something that potentially could scare HAMs from trying experiments with the firmware.

#### Instating a release master
[PA3MET](https://www.qrz.com/db/PA3MET) took up the challenge to, on his own fork of Travis' repository, merge viable commits and create working firmware. A lenghty and labor-intensive process as after merging there would be building, testing and uploading. PD2TMS joined in and using [Jenkins](https://jenkins-ci.org) and a dedicated build-machine created automated pre-releases on a website. Talking about this on DMR, our [favourite analogue repeater](http://www.pi2nos.nl) and in Telegram the idea was highly appreciated by both committors, *including but not limited* to [DF8AV](https://www.qrz.com/db/DF8AV), [PH7WIM](https://www.qrz.com/db/PH7WIM) & [PA9TV](https://www.qrz.com/db/PA9TV), but also by [PA3PM](https://www.qrz.com/db/PA3PM) for publication on his news-site [HAM-DMR](http://ham-dmr.nl).

#### Upgrading and automating
Using the experience from the initial Jenkins setup, PD2TMS tested with both Jenkins and Travis (tooling!) to get releases back into the Github repository. Although more straighforward the Travis solution was put on hold and the Jenkins solution further [documented and deployed](JenkinsBuilder.md). The current version (end of sept. '16) will 

  * On every commit to the [release master repository](https://github.com/roelandjansen/md380tools) start the process
  * Inform us of the commit
  * Determine changes merged
  * Build new firmware and CSV files
  * Pre-release the software at the [repository release page](https://github.com/roelandjansen/md380tools/releases)
  * Inform us of the now succesfully pre-released firmware
 
#### Quality Assurance
The release master and all others in the group informed can now test the firmware on their MD380 radio's and inform the release master. Upon succesfull tests he will change the pre-release to release so it's clearly marked in green as something viable to run and relatively safe to install. **Do note** that actually using the experimental firmware and especially any new experimental features still is an 'at your own risk' task. Nobody but the HAM requesting someone to, or performing her-/himself, can be held accountable for any damage to the unit.

73's and happy experimenting, [PD2TMS](https://www.qrz.com/db/PD2TMS) (Tom)
