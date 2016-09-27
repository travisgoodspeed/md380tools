Automated building (of MD380tools) using Jenkins and a dedicated buildbox
=====

FAQ:

- Why? Convenience
- Why dedicated buildbox? Because my Jenkins was doing other stuff on a non-buildtooling CentOS box and that's not capable of running ARM cross compilers


Prepare
====

Have a Jenkins installation somewhere (what, no details -> no just google that, learn how jenkins works)! Not being mean, but Jenkins is too much to discuss, althouh configuration of the job will be explained!

Ensure Jenkins :
 - has a private SSH key present. Copy it's public key somewhere you can grab it for the 'install' part.
 - is able to talk to Github (firewall, etc.) and that github can talk to http(s!)://yoururl/github-webhook/ unobstructed. Note that this should be relatively safe as the actual Jenkins plugin will verify each hook-call with github before acting (so cheating won't work).
 - can communicate (ssh) to your build-environment

Log in to (or ask the other party who commits to) github
 - Click on your profile picture (right top)
 - Click on settings on the opening box
 - Click to your left on 'Personal access tokens'
 - SAVE your api-token (it won't be displayed again)
 - Create a token, name it and mark 'repo' and 'admin:repohook'
 - SAVE your api-token (it won't be displayed again)

So, you now should have a couple things in your notepad
 - Copy of a single-lined jenkins public key
 - Github API token
And
 - Name of your user and repository on github
 - Copy of the url of your github (e.g. https://github.com/user/myrepo)

Install & configure
====
Basic OS installation of Debian Stretch (really, yeah, really! in true Dunglish 'omdat het kan'). Added a user md380 during install.

This box needs
 - incoming traffic from Jenkins
 - outgoing traffic (80/443 TCP) for webcalls to retrieve the github repositories & firmware downloads from elsewhere

Steps
 - Log in as ```md380```
 - Become root ```su -```
 - Install sudo ```apt-get install sudo && usermod -G sudo md380```
 - Log out & back in again (so your groups are applied)
 - Copy the jenkins pubkey into your clipboard as a SINGLE LINE and ```mkdir ~/.ssh ; echo "PASTEYOURKEYHERE" > ~/.ssh/authorized_keys ; chmod 700 ~/.ssh ; chmod 600 ~/.ssh/authorized_keys```
 - Get the builder to work on itself
 - ```sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi libusb-1.0 python-usb unzip curl zip git make golang```
 - Next get github-release(r) ready ```sudo GOPATH=$HOME/go go get github.com/aktau/github-release```


Jenkins job
===
Ensure you have git/github plugins/parameterized builds installed, personally I'd add ansi terminal, green balls (who ever wanted blue ones) and console timer to it.

Create a new credential as 'secret text' naming it so you know it's your github api-token and paste the token as the secret

Create a new node, point it to the builder box (as per SSH-key it should install the slave on it)

In main configuration add a 'github server' accept the normal proposed url and in the dropdown box select your token.

Create a new free-form job
 - Add parameterized 'GITHUB_ORGANIZATION' (string),'GITHUB_REPO' (string), 'GITHUB_TOKEN' (password), 'PROJECT_NAME' (string)
 - The first is your github user, the second obviously the repo, the third your acquired key, the fourth something you'd like to reference your release by
 - Recommendation, mark 'delete old builds' and set it at 30/100 or something likewise
 - Add in SCM a 'git' repo, pointing to your github url
 - Mark 'Build when changed pushed to github' -> if this shows nothing (as in no yellow line added) check your github account, repo, settings and then 'hooks', there should be something! If the yellow line appears, check your api-token settings on github & doublecheck your jenkins main configuration settings on github
 - I'd recommend setting abort the build if it's stuck to a sane amount of minutes (like 10 if the curl-downloads tend to be slow) + add timestamps + color ansi output (if you have those plugins installed)
 - Now for the builders, add two shell-scripts & a mail after building block. The latter being the simplest (fill in your mailaddress)

Builder 1
```
make clean && make
cd db ; make clean && make all
```

Builder 2 (and yes is it ugly but working
```
export SHORT_GIT_COMMIT=`echo ${GIT_COMMIT} | cut -c 1-7`
export FILE=experiment-${SHORT_GIT_COMMIT}-${BUILD_NUMBER}-`date +%Y%m%d%H%M%S`.bin
export PREVTAG="`git log --format="%h" --first-parent -n 1 --skip=1`"
export COMMITMSG="`git log --format="%h: %B" ${PREVTAG}..HEAD  --no-merges`"

# Clean up older copies if make didnapos;t do that
rm experiment-*-*.bin || echo "Nothing to delete here :)"
cp applet/experiment.bin ${FILE}

#####
# Publish to example.org/md380
#####
echo "cd /var/www/example.org/md380
put ${FILE}" | sftp foo@example.org

#####
# Publish to GitHub repo
#####
echo "Deleting release from github before creating new one"
/go/bin/github-release delete --user ${GITHUB_ORGANIZATION} --repo ${GITHUB_REPO} --tag ${SHORT_GIT_COMMIT} || echo "No such version, continuing"

echo "Creating a new release in github"
/go/bin/github-release release --user ${GITHUB_ORGANIZATION} --repo ${GITHUB_REPO} --tag ${SHORT_GIT_COMMIT} --name "${SHORT_GIT_COMMIT}" --description "${COMMITMSG}" --pre-release

echo "Uploading the artifacts into github"
/go/bin/github-release upload --user ${GITHUB_ORGANIZATION} --repo ${GITHUB_REPO} --tag ${SHORT_GIT_COMMIT} --name "${SHORT_GIT_COMMIT}-${PROJECT_NAME}-firmware.bin" --file ${FILE}
/go/bin/github-release upload --user ${GITHUB_ORGANIZATION} --repo ${GITHUB_REPO} --tag ${SHORT_GIT_COMMIT} --name "${SHORT_GIT_COMMIT}-users.csv" --file db/users.csv
/go/bin/github-release upload --user ${GITHUB_ORGANIZATION} --repo ${GITHUB_REPO} --tag ${SHORT_GIT_COMMIT} --name "${SHORT_GIT_COMMIT}-repeaters.csv" --file db/repeaters.csv
```

**Now commit and see the magic working**

Cheers, Tom / PD2TMS
