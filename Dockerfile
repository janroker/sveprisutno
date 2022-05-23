# CLion remote docker environment (How to build docker container, run and stop it)
#
# Build and run:
#   docker build -t esp32:0.1 -f Dockerfile .
#   docker run -d --cap-add sys_ptrace -p127.0.0.1:2222:22 -v /home/janroker/Documents/git:/root/git --device=/dev/ttyUSB0 --name esp32 esp32:0.1         
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:2222"
# 	
# 	for root ssh: 
#		use password="password" or
#   	add pub_key as commented below -> line 69
#	
# 	docker exec -it esp32 /bin/sh
#	passwd root
#   ssh root@127.0.0.1 -p 2222
#
# stop:
#   docker stop esp32
# 

FROM ubuntu:20.04

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata

RUN apt-get update \
  && apt-get install -y ssh \
      build-essential \
      gcc \
      g++ \
      gdb \
      clang \
      make \
      ninja-build \
      cmake \
      autoconf \
      automake \
      locales-all \
      dos2unix \
      rsync \
      tar \
      python \
      git \ 
      wget \
      flex \
      bison \
      gperf \
      python3 \
      python3-pip \
      python3-setuptools \ 
      ccache \
      libffi-dev \
      libssl-dev \
      dfu-util \
      libusb-1.0-0 \
      nano \
  && apt-get clean

RUN ( \
    echo 'LogLevel DEBUG2'; \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Subsystem sftp /usr/lib/openssh/sftp-server'; \
  ) > /etc/ssh/sshd_config_test_clion \
  && mkdir /run/sshd

RUN useradd -m user \
  && yes password | passwd user \
  && yes password | passwd root

# uncomment for ssh pubkey auth...  
#RUN echo "PASTE_PUB_KEY" >> /root/.ssh/authorized_keys

RUN usermod -s /bin/bash user
RUN usermod -a -G dialout root && usermod -a -G dialout user

RUN mkdir -p /root/esp && cd /root/esp \
	&& git clone --recursive https://github.com/espressif/esp-idf.git \
	&& cd /root/esp/esp-idf && ./install.sh esp32
	
RUN echo ". /root/esp/esp-idf/export.sh" >> /etc/bash.bashrc

CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_test_clion"]
