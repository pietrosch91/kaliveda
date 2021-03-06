#include "KVConfig.h"
#ifdef WITH_RSQLITE
#include "SQLiteDB.h"
#include <KVNumberList.h>

void sqlite_example()
{
   // Opening/creating a database
   // If database does not exist on disk, it will be created
   KVSQLite::database db("example.sqlite");

   if (!db.get_number_of_tables()) {
      // no tables in database => it has not been created before

      // define and add a table to database
      KVSQLite::table cars("Cars");
      cars.add_primary_key("Id");
      cars.add_column("Name", "TEXT");
      cars.add_column("Price", "INTEGER");
      db.add_table(cars);
      // This is equivalent to
      //   CREATE TABLE IF NOT EXISTS Cars(Id INTEGER PRIMARY KEY, Name TEXT, Price INTEGER)
      TString names[] = {"Audi", "Mercedes", "Citroen", "Volvo", "Bentley", "Citroen", "Audi", "Volkswagen", "-"};
      int prices[] = {52642, 57127, 9000, 29000, 350000, 21000, 41400, 21600};

      // fill table with some data
      db.prepare_data_insertion("Cars");
      int row = 0;
      while (names[row] != "-") {
         db["Cars"]["Name"].set_data(names[row]); // N.B. do not use the 'cars' variable
         db["Cars"]["Price"] = prices[row]; // use 'set_data' or simply '=' to fill columns in row
         db.insert_data_row();
         ++row;
      }
      db.end_data_insertion();
      // Now our table looks like this:
      //      Id          Name        Price
      //      ----------  ----------  ----------
      //      1           Audi        52642
      //      2           Mercedes    57127
      //      3           Citroen     9000
      //      4           Volvo       29000
      //      5           Bentley     350000
      //      6           Citroen     21000
      //      7           Audi        41400
      //      8           Volkswagen  21600

      // Add a table with incomplete information: some column values are 'NULL'
      // as if they had never been filled
      KVSQLite::table t("Tasks");
      t.add_column("Number", KVSQLite::column_type::INTEGER);
      t.add_column("Worker", KVSQLite::column_type::TEXT);
      db.add_table(t);

      int numbers[] = {221, 222, 245, 250, 321, 333, -1};
      TString wrks[] = {"WRK01", "", "WRK02", "WRK01", "", "WRK03"};
      db.prepare_data_insertion("Tasks");
      row = 0;
      while (numbers[row] > 0) {
         db["Tasks"]["Number"] = numbers[row];
         if (wrks[row] != "") {
            db["Tasks"]["Worker"] = wrks[row];
         } else {
            db["Tasks"]["Worker"].set_null();// N.B. this is not the same as filling with an empty string
         }
         db.insert_data_row();
         ++row;
      }
      db.end_data_insertion();
   }

   // Retrieving and selecting data
   // 1. All unsorted data
   // print contents of table
   std::cout << "SELECT * FROM Cars\n" << std::endl;
   for (int i = 0; i < db["Cars"].number_of_columns(); ++i) // print column names
      std::cout << db["Cars"][i].name() << "\t\t\t";
   std::cout << std::endl;

   db.select_data("Cars"); //  ====>  SELECT * FROM Cars
   while (db.get_next_result()) {
      std::cout << db["Cars"]["Id"].get_data<int>()
                << "\t\t\t" << db["Cars"]["Name"].get_data<TString>()
                << "\t\t\t" << db["Cars"]["Price"].get_data<int>()
                << std::endl;
   }
   std::cout << std::endl << std::endl;

   // 2. Sorted data
   std::cout << "SELECT * FROM Cars ORDER BY Price\n" << std::endl;
   db.select_data("Cars", "*", "", false, "ORDER BY Price"); //  ====>  SELECT * FROM Cars ORDER BY Price
   // we can use a reference to the Cars table
   KVSQLite::table& Cars = db["Cars"];
   while (db.get_next_result()) {
      std::cout << Cars["Id"].get_data<int>()
                << "\t\t\t" << Cars["Name"].get_data<TString>()
                << "\t\t\t" << Cars["Price"].get_data<int>()
                << std::endl;
   }
   std::cout << std::endl << std::endl;

   // 3. Unique column values
   std::cout << "SELECT DISTINCT Name FROM Cars\n" << std::endl;
   db.select_data("Cars", "Name", "", true); //  ====>  SELECT DISTINCT Name FROM Cars
   while (db.get_next_result()) {
      std::cout << Cars["Name"].get_data<TString>() << ", ";
   }
   std::cout << std::endl << std::endl;

   // 4. Selecting data
   db.select_data("Tasks", "Number", "Worker=\"WRK01\"");
   std::cout << "Task numbers for worker WRK01: ";
   KVNumberList nl;
   while (db.get_next_result()) {
      nl.Add(db["Tasks"]["Number"].get_data<int>());
   }
   std::cout << nl.AsString() << std::endl << std::endl;
   // using KVNumberList for numerical column selection
   db.select_data("Tasks", "*", nl.GetSQL("Number"));
   while (db.get_next_result()) {
      std::cout << "Number=" << db["Tasks"]["Number"].get_data<int>() <<
                " Worker=" << db["Tasks"]["Worker"].get_data<TString>() << std::endl;
   }

   nl.Clear();
   db.select_data("Tasks", "Number", "Worker IS NULL");
   std::cout << "Task numbers with no worker: ";
   while (db.get_next_result()) {
      nl.Add(db["Tasks"]["Number"].get_data<int>());
   }
   std::cout << nl.AsString() << std::endl << std::endl;

   // 5. Counting
   std::cout << "The number of unique Worker names in table Tasks is " <<
             db.count("Tasks", "Worker", "", true)
             << std::endl;  // prints 3, i.e. "WRK01", "WRK02", and "WRK03" (NULL values not counted when count(Worker) is used)
   std::cout << "The number of rows with no Worker name in table Tasks is " <<
             db.count("Tasks", "*", "Worker IS NULL")
             << std::endl;  // prints 2 (NULL values counted when count(*) is used)

   // 6. Updating a table: change all 'NULL' Worker values to "WRK04"
   db["Tasks"]["Worker"].set_data("WRK04");
   db.update("Tasks", "Worker", "Worker IS NULL");
   std::cout << "The number of unique Worker names in table Tasks is " <<
             db.count("Tasks", "Worker", "", true)
             << std::endl;  // prints 4, i.e. "WRK01", "WRK02", "WRK03", and "WRK04"
   std::cout << "The number of rows with no Worker name in table Tasks is " <<
             db.count("Tasks", "*", "Worker IS NULL")
             << std::endl;  // prints 0 (all NULL values were replaced)

}
#endif
