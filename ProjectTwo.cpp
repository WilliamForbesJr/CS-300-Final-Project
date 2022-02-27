// ProjectTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <functional> //for std::hash

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 100;

// define a structure to hold course information
struct Course {
    string courseId; // unique identifier
    string title;
    vector<string> prerequisites;
};

// Forward Declaration
void quickSort(vector<Course>& courses, int begin, int end);

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold courses
    struct Node {
        Course course;
        unsigned key;
        Node* next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a course
        Node(Course _course) : Node() {
            course = _course;
        }

        // initialize with a course and a key
        Node(Course _course, unsigned aKey) : Node(_course) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;
    unsigned int hash(string courseId);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    void Remove(string courseID);
    Course Search(string courseID);
    void ValidatePrerequisites();
};



//============================================================================
//                      Hash Table Methods
//============================================================================

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    tableSize = size;
    // resize nodes size
    nodes.resize(tableSize);
}

/**
 * Destructor
 */
HashTable::~HashTable() {
    // erase nodes beginning
    nodes.erase(nodes.begin());
}

/**
 * Take courseId param and using the c++ std::hash, hash the string into an integer.
 * We then use that integer for hashing specific to the table.
 *
 * @param courseId
 * @return The calculated hash
 */
unsigned int HashTable::hash(string courseId) {
    // Instantiate the standard library hash function
    std::hash<string> hasher;

    // take courseId, hash it to a size_t and cast to an integer
    unsigned int hashed = static_cast<unsigned int>(hasher(courseId));

    return hashed % tableSize;
}

/**
 * Insert a course
 *
 * @param course The course to insert
 */
void HashTable::Insert(Course course) {
    // create the key for the given course
    unsigned key = hash(course.courseId);
    // retrieve node using key
    Node* prevNode = &(nodes.at(key));

    // if no entry found for the key assign this node to the key position
    if (prevNode == nullptr) {
        Node* nextNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*nextNode));
    }
    else {
        // else if node is not used
    // assing old node key to UNIT_MAX, set to key, set old node to course and old node next to null pointer
        if (prevNode->key == UINT_MAX) {
            prevNode->key = key;
            prevNode->course = course;
            prevNode->next = nullptr;
        }

        else {
            // if not found, find the next node available
            while (prevNode->next != nullptr) {
                prevNode = prevNode->next;
            }

            //unsigned newKey = prevNode->key++;
           // Node* newNode = new Node(course, key);
           // nodes.insert(nodes.begin() + newKey, (*newNode));
            prevNode->next = new Node(course, key);
        }
    }
}


/**
 * Remove a course
 *
 * @param courseId The course id to search for
 */
void HashTable::Remove(string courseId) {
    // set key equal to hash atoi courseID cstring
    // erase node begin and key
    unsigned key = hash(courseId);
    nodes.erase(nodes.begin() + key);
}


/**
 * Search for the specified courseId
 *
 * @param courseId The course id to search for
 */
Course HashTable::Search(string courseId) {
    Course course;

    // create the key for the given course
    unsigned key = hash(courseId);
    Node* node = &(nodes.at(key));

    // if no entry found for the key return course
    if (node == nullptr || node->key == UINT_MAX) {
        return course;
    }

    // if entry found for the key return node course
    if (node != nullptr && node->key != UINT_MAX && courseId.compare(node->course.courseId) == 0) {
        return node->course;
    }

    // while node not equal to nullptr
        // if the current node matches, return it
        //node is equal to next node
    while (node != nullptr) {
        if (node->course.courseId.compare(courseId) == 0) {
            return node->course;
        }

        node = node->next;
    }

    return course;
}


//============================================================================
//                          Print Methods
//============================================================================

/**
 * Print all courses sorted by courseId
 */
void HashTable::PrintAll() {
    vector<Course> courseList;

    for (unsigned int i = 0; i < nodes.size(); ++i) {
        if (nodes[i].key != UINT_MAX) {
            courseList.push_back(nodes[i].course);
            Node* node = &(nodes[i]);

            while (node->next != nullptr) {
                node = node->next;
                courseList.push_back(node->course);
            }
        }
    }

    quickSort(courseList, 0, courseList.size() - 1);

    for (unsigned int i = 0; i < courseList.size(); ++i) {
        cout << courseList[i].courseId << ": " << courseList[i].title << endl;
    }

    cout << endl;
}

/* 
 * Dispaly message to user if course contains invalid prerequisites 
 */
void printInvalidPrerequisites(Course course, string invalidId) {
    cout << "\nThe following course has invalid prerequisites: \n"
        << course.courseId << " " << course.title << "\n"
        << invalidId << " does not exist \n"
        << "It will not be added. Please review the data and try again\n" << endl;
}

/* 
 * Helper Method to Print User Menu
 */
void renderMenu() {
    cout << "Welcome to the course planner\n" << endl;

    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;

    cout << "\nWhat would you like to do ?" << endl;
}

/**
 * Display the course information to the console (std::out)
 *
 * @param course struct containing the course info
 */
void displayCourse(Course course) {
    cout << course.courseId << ": " << course.title << endl;

    cout << "Prerequisites: ";

    for (int i = 0; i < course.prerequisites.size(); i++) {
        cout << course.prerequisites[i] << ", ";
    }

    cout << "\n" << endl;
}

/* 
 * Prompt user for search term and use result to find course within hashtable
 */
void searchForCourse(HashTable* hashtable) {
    string searchTerm;

    cout << "Which course do you want to know about? ";

    cin >> searchTerm;

    // Normalize string to uppercase value for case-insensitive searching
    for (int i = 0; i < searchTerm.size(); i++) {
        searchTerm.at(i) = toupper(searchTerm.at(i));
    }

    Course result = hashtable->Search(searchTerm);

    // If we return an empty course (not found). Output message
    if (result.title.size() == 0) {
        cout << "Course not found." << endl;
    }

    // Otherwise, display course
    else {
        displayCourse(result);
    }
}


//============================================================================
//                          Validation Methods
//============================================================================

void HashTable::ValidatePrerequisites() {
    for (unsigned int i = 0; i < nodes.size(); i++) {
        vector<string> preRequisites = nodes[i].course.prerequisites;

        if (nodes[i].key != UINT_MAX && preRequisites.size() > 0) {
            for (unsigned int j = 0; j < preRequisites.size(); j++) {
                Course found = Search(preRequisites[j]);

                if (found.courseId.empty()) {
                    printInvalidPrerequisites(nodes[i].course, preRequisites[j]);
                    Remove(nodes[i].course.courseId);
                    break;
                }
            }
        }
    }
}

/*
 * Takes hashtable param and returns a boolean to determine if hashtable exists or not.
 * If nullptr, hashtable has not loaded.
 */
bool validateFileLoaded(HashTable* hashtable) {
    if (hashtable == nullptr) {
        cout << "\n\nERROR: Please load the data from file\n\n" << endl;
        return false;
    }

    return true;
}


//============================================================================
//                         Vector Sorting Methods
//============================================================================


/**
 * Partition the vector of courses into two parts, low and high
 *
 * @param courses Address of the vector<course> instance to be partitioned
 * @param begin Beginning index to partition
 * @param end Ending index to partition
 */
int partition(vector<Course>& courses, int begin, int end) {
    //set low and high equal to begin and end
    int low = begin;
    int high = end;

    // pick the middle element as pivot point
    int mid = low + (high - low) / 2;
    Course pivot = courses.at(mid);

    bool done = false;

    while (!done) {
        // keep incrementing low index while courses[low] < courses[pivot]
        while (courses.at(low).courseId < pivot.courseId) {
            low += 1;
        }

        // keep decrementing high index while courses[pivot] < courses[high]
        while (pivot.courseId < courses.at(high).courseId) {
            high -= 1;
        }

        /* If there are zero or one elements remaining,
         * all courses are partitioned. Return high
         */
        if (low >= high) {
            done = true;
        }

        // else swap the low and high courses (built in vector method)
        else {
            swap(courses.at(low), courses.at(high));
            // move low and high closer ++low, --high
            ++low;
            --high;
        }
    }

    return high;
}


/**
 * Perform a quick sort on course title
 *
 * @param courses address of the vector<course> instance to be sorted
 * @param begin the beginning index to sort on
 * @param end the ending index to sort on
 */
void quickSort(vector<Course>& courses, int begin, int end) {
    int mid = 0;

    /* Base case: If there are 1 or zero courses to sort,
     * partition is already sorted otherwise if begin is greater
     * than or equal to end then return
     */
    if (begin >= end) {
        return;
    }

    // Partition courses into low and high
    mid = partition(courses, begin, end);

    // recursively sort low partition 
    quickSort(courses, begin, mid);

    // recursively sort high partition
    quickSort(courses, mid + 1, end);
}


void loadStuff(HashTable* hashtable) {
    fstream fin;

    fin.open("data.csv", ios::in);

    string line, item;

    while (getline(fin, line))
    {
        Course course;
        stringstream str(line);

        int index = 0;

        while (getline(str, item, ',')) {
            if (index == 0) {
                course.courseId = item;
            }

            if (index == 1) {
                course.title = item;
            }

            if (index > 1) {
                course.prerequisites.push_back(item);
            }

            index++;
        }

        if (index < 2) {
            cout << "\nInvalid Data: " << line << "\n"
                 << "Please review the data and try again.\n" 
                 << endl;
        }

        else {
            hashtable->Insert(course);
        }
    }
}

int main(int argc, char* argv[]) {
    // Define a hash table to hold all the courses
    HashTable* courseTable = nullptr;
    Course course;

    int choice = 0;

    while (choice != 9) {
        renderMenu();
        cin >> choice;

        switch (choice) {

        case 1:
            courseTable = new HashTable();
            loadStuff(courseTable);
            courseTable->ValidatePrerequisites();
            break;

        case 2:
            if (validateFileLoaded(courseTable)) {
                courseTable->PrintAll(); 
            }
            break;

        case 3:
            if (validateFileLoaded(courseTable)) {
                searchForCourse(courseTable);
            }
            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
