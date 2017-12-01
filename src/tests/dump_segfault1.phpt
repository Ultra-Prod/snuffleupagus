--TEST--
Disable functions check on `ret` with an alias
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_dump_segfault1.ini
--FILE--
<?php 
echo strpos("pouet", "p") . "\n";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The execution has been aborted in %a/tests/dump_segfault1.php:2, because the function 'strpos' returned '0', which matched the rule 'test'.

