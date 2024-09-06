git pull
mpiCC -O3 -I/home/desposito/.lib2/ main.c loop.c kernel.c -L/home/desposito/.lib2/ -lpvm
mpirun --job-name="prova" -p parallel -n "5" a.out > srun.log 2>&1 &
jobs
mpirun -np "5" --hostfile pippo a.out
mpirun -np "5" a.out
mpirun -np "5" ./a.out
mpirun -np "5" ./a.out > srun.log 2>&1 &
mpirun --output-filename output_%t.txt -np "5" ./a.out > srun.log 2>&1 &
jobss
jobs
./lxgriv-run.sh 
screen -r Corr_tw1
git status
git add -A
git commit -m "New code"
cd ..
touch .gitconfig
cd es
cd desposito/
touch .gitconfig
./lxgriv-run.sh 
screen -r Corr_tw1
git fetch
./lxgriv-run.sh 
screen -r Corr_tw1
git status
git pull
git status
git branch
git pul
git pull
git branch
git checkout lxgriv11
git status
git add -A
git commit -m "New branch"
git config --global user.name "DarioHub95"
git config --global user.name "DarioHub95"git config --global user.email "despositosogei@gmail.com"
git config --global user.email "despositosogei@gmail.com"
git commit -m "New branch"
git push
