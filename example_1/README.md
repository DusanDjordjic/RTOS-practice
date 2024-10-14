# Example 1

Count how many characters user typed.

Idea is to have a semaphore that we are using for locking the counter thread until user types something.
For that we are using `sem_t semaphore`. 

Second semaphore called done we are using to send a signal to a thread that we are done with reading the chars. 
After that we are calling `pthread_join(thread, NULL)` to wait for thread to finish.

With only these two semapthores we will run into locking problem. 
The thing is that after we are done with reading from stdin its not enough to just call `sem_post` on done semaphore 
because currently out thread is waiting for "sem" semaphore. We need to post to both of these semaphores in order
to shutdown the thread.

This all works fine if user types characters one by one but if he types for example "abcdq" our thread will only catch
the 'a' because main thread reads the data so fast and reads the 'q' before second theread does all the processing
and sends the exit signal, so we need one more signal that we can use to signalize main thread that we are done with 
processing (incrementing count) and it can startreading from stdin again.
