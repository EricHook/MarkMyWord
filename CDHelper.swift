//
//  CDHelper.swift
//  coreDataTests
//
//  Created by Eric Hook on 11/30/15.
//  Copyright © 2015 Eric Hook. All rights reserved.
//

import Foundation
import CoreData

private let _sharedCDHelper = CDHelper()
class CDHelper : NSObject  {
    
    // MARK: - SHARED INSTANCE
    class var shared : CDHelper {
        return _sharedCDHelper
    }
 
    
    // MARK: - PATHS
    lazy var storesDirectory: NSURL = {
        let fm = NSFileManager.defaultManager()
        let urls = fm.URLsForDirectory(.DocumentDirectory, inDomains: .UserDomainMask)
        return urls[urls.count-1] as NSURL
    }()

    
    lazy var localStoreURL: NSURL = {
        let url = self.storesDirectory.URLByAppendingPathComponent("LocalStore.sqlite")
        print("localStoreURL = \(url)")
        return url
    }()
    
    
    lazy var modelURL: NSURL = {
        let bundle = NSBundle.mainBundle()
        if let url = bundle.URLForResource("Model", withExtension: "momd") {
            return url
        }
        print("CRITICAL - Managed Object Model file not found")
        abort()
    }()

    
    // MARK: - CONTEXT
    lazy var context: NSManagedObjectContext = {
        let moc = NSManagedObjectContext(concurrencyType:.MainQueueConcurrencyType)
        moc.persistentStoreCoordinator = self.coordinator
        return moc
    }()
    
    
    // MARK: - MODEL
    lazy var model: NSManagedObjectModel = {
        return NSManagedObjectModel(contentsOfURL:self.modelURL)!
    }()
    
    
    // MARK: - COORDINATOR
    lazy var coordinator:NSPersistentStoreCoordinator = {
        return NSPersistentStoreCoordinator(managedObjectModel:self.model)
    }()

    
    // MARK: - STORE
    lazy var localStore: NSPersistentStore? = {
        let options = [NSSQLitePragmasOption:["journal_mode":"DELETE"]]
        var error: NSError?
        var _localStore : NSPersistentStore?
        do {
            
            let _localStore = try self.coordinator.addPersistentStoreWithType(NSSQLiteStoreType, configuration: nil, URL: self.localStoreURL, options: options)   //  NSSQLiteStoreType // NSXMLStoreType
        }

        catch let error as NSError {
            print("ERROR creating store at '\(self.localStoreURL)'")
        }
        return _localStore
    }()

    
    // MARK: - SETUP
    required override init() {
        super.init()
        self.setupCoreData()
    }
    func setupCoreData() {
        _ = self.localStore
    }
    

    // MARK: - SAVING
    class func save (moc:NSManagedObjectContext!) {
        
        if moc.hasChanges == false {
            print("SKIPPED save, context \(moc) has no changes")
            return
        }
        //var error: NSError? = nil
        
        do {
            try moc.save()

            print("SAVED context \(moc)")
            
            if moc.parentContext != nil {
                save(moc.parentContext)
            }
            return
            
        }
            
        catch let error as NSError {
            print("NSPersistentStore save() Error: \(error.localizedDescription)")
        }
        
//        if let _error = error {
//            print("FAILED TO SAVE. \(_error.localizedDescription)")
//        }
    }
    
    
    class func saveSharedContext () {
        CDHelper.save(CDHelper.shared.context)
    } 
}


