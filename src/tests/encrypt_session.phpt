--TEST--
SEssion cookie encryption
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/session_cookie_encryption.ini
--FILE--
<?php
session_start();
if (!isset($_SESSION['count'])) {
  $_SESSION['count'] = 1337;
} else {
  $_SESSION['count']++;
}
echo "count: " . $_SESSION['count'];
?>
--EXPECTF--
count: 1337
