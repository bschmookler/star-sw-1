#! /usr/local/bin/tcsh -f
source /star/u/starreco/.tcshrc 
source /afs/rhic/rhstar/group/.starnew
setenv NODEBUG yes
starver 01g
echo "Start $0 with $argv on" `date`
perl $STAR/mgr/bfc $argv

