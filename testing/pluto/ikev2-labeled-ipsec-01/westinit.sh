/testing/guestbin/swan-prep
echo 3 > /proc/sys/net/core/xfrm_acq_expires
# install selinux; generated in OUTPUT by east
semodule -i -v OUTPUT/ipsecspd.pp
# start pluto
ipsec start
../../guestbin/wait-until-pluto-started
ipsec auto --add labeled
echo "initdone"
