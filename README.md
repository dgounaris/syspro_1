
Implementation of a bitcoin transaction system for 1st system programming assignment.

In order to compile the program run "make" inside the directory. Run by running the command "./main" in the same directory.
Make sure to pass the correct file arguments to the program by using the parameters described below:
-a [char*]: The wallets file
-t [char*]: The transactions file
-v [int]: The bitcoin value
-h1 [int]: Sender hash table number of entries
-h2 [int]: Receiver hash table number of entries
-b [int]: Bucket size (in bytes)

The implementation consists of 5 main parts:
- The main file, responsible for receiving and handling the incoming commands, as well as instantiating and deleting relevant structures.
- The transaction file, containing transaction structures declaration and related functions.
- The bc_transaction file, containing structures regarding bitcoin specific data for transactions
- The wallet file, containing structures regarding user specific data, such as current account balance
- The service file, containing the transaction logic implementation, synchronization and validation

The data structures used are the following:
- 1 hash table containing the transactions per sending user (as a list), hashed by sending wallet id
- 1 hash table containing the transactions per receiving user (as a list), hashed by receiving wallet id
- 1 tree per bitcoin, containing its transation history
- 1 hash table containing the bitcoin trees in order to easily retrieve them (hashed by bitcoin id)

Designed and implemented by D. Gounaris.
