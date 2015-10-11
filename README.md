# Scorpion (Dev Branch)
The Scorpion programming language

#Rules of The Road
The `dev` branch is used for ALL Scorpion development. DO NOT use any other branch for Scorpion development other than this branch.

###How to use this branch?
This branch is used in 1 week intervals. Meaning that Scorpion is developed for 1 week and then bushed to production. There must be at least 2 days allocated for testing.

You MUST test the code before pushing it to production. The `master` branch is used for production code and will ONLY be used for that purpose, i.e the master branch will only be updated weekly.

Once your code has been developed tested and is ready for production. Create a new release for the master branch and push the code w/ that release. After the released has been pushed, update `dist` branch with the same release number you pushed in master branch to distribute the update for the Scorpion programming language. 

After all that hard work is done simply repeat the step to update dist branch for the Scorpion-Dist Repo.

#Author
Scorpion was originally developed and designed by Braxton Nunnally in 2015.
