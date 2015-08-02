$env:TERM="msys";

$version = "0x" + (git log --format="%H" -n 1).substring(0,8);

$githash = git log --format="%H" -n 1;
$gitinfo = git describe --tags;
$gitbranch = git rev-parse --abbrev-ref HEAD;

$revcount=[int](git rev-list --count master) - [int](git rev-list --count master ^HEAD);