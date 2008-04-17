
#include <iostream>

#include "base/util/Enumeration.h"
#include "base/util/KeyValuePair.h"
#include "client/SQLiteKeyValueStore.h"
#include "base/globalsdef.h"

USE_NAMESPACE

int main()
{
    SQLiteKeyValueStore kvs("test", "key", "value", "test.db");
    
    kvs.connect();
    
    Enumeration& en = kvs.getProperties();

    int x = 0;
    while (en.hasMoreElement())
    {
        KeyValuePair * kvp = (KeyValuePair*) (en.getNextElement());
        if (strcmp(kvp->getKey(), "2") == 0)
        {
            int ret = kvs.removeProperty(kvp->getKey());
            std::cout << "Attempt to remove " << kvp->getKey() << ": " << ret << std::endl;
        }
        

        std::cout << "Element " << x << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
    }
    
    StringBuffer test;
    test = kvs.readPropertyValue("2");
    if (test.null())
    {
        std::cout << "Correct!" << std::endl;
    }
    
    kvs.save();
    
    test = kvs.readPropertyValue("2");
    if (test.null())
    {
        std::cout << "Correct!" << std::endl;
    }
    
    Enumeration& en2 = kvs.getProperties();

    x = 0;
    while (en2.hasMoreElement())
    {
        KeyValuePair * kvp = (KeyValuePair*) (en2.getNextElement());
        if (strcmp(kvp->getKey(), "2") == 0)
        {
            int ret = kvs.removeProperty(kvp->getKey());
            std::cout << "Attempt to remove " << kvp->getKey() << ": " << ret << std::endl;
        }
        std::cout << "Element " << x << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
    }
    
    kvs.disconnect();
    
    return 0;
}   