FROM eecs388/baseimage-gui:debian-11

RUN add-pkg firefox-esr=91.13.0esr-1~deb11u1
RUN apt-mark hold firefox-esr

RUN add-pkg software-properties-common && \
	apt-add-repository contrib

RUN echo ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true | debconf-set-selections
RUN add-pkg ttf-mscorefonts-installer

COPY rootfs/ /

ENV APP_NAME="CPSC 4200 Firefox"

# If you need to do any additional setup for development
# in the container like installing additional programs,
# do it in this file so that the changes are persisted.
#
# Example: to install Emacs, we might add:
#    RUN sudo apt-get update && sudo apt-get install -y emacs

