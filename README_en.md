# c_transaction

pure C transaction frame, can solve sync and asyn transaction sequential actions; 

## todo feature：
* more test case
* mem check when use heap mem
* doc
* log
* example

## support feature
* action context
* rollback when action fail
* sub trans(with roll back)
* asyn with sub trans
* can alloc mem from static mem or heap(default is heap)
* mem check when use static mem

## Test
use https://github.com/cpputest/cpputest as UT frame and Mock frame ,because only this  frame can easy support mac os mock static function.
