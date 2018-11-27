date /t >  all-tests-gmp-6.1.2.txt
time /t >> all-tests-gmp-6.1.2.txt
set PATH=..\;%PATH%
for /R %%f in (*.exe) do %%f || echo %%f fail >> all-tests-gmp-6.1.2.txt
