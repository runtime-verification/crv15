#!/bin/bash
make -C ./CRVBenchMarkSrv/Debug/ clean
make -C ./CRVBenchMark/Debug/ clean
make -C ./CRVBenchMarkSrv/Debug/
make -C ./CRVBenchMark/Debug/ 
./CRVBenchMarkSrv/Debug/CRVBenchMarkSrv &
./CRVBenchMark/Debug/CRVBenchMark
#Important : The process of CRVBenchMarkSrv needs to be stopped manually if the script is to be rerun
