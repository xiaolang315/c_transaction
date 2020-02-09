# c_transaction

pure C transaction frame, can solve most transaction issue; 

## todo feature：
* more test case
* mem check
* try unify asyn and sync 

## support feature
* action context
* rollback
* sub trans(with roll back)
* asyn with sub trans

## Test
use https://github.com/cpputest/cpputest as UT frame and Mock frame ,because only this  frame can easy support mac os mock static function.
