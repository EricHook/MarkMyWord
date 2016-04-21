//
//  CoreDataStack.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/18/16.
//  Copyright © 2016 Hook Studios. All rights reserved.
//

import Foundation
import CoreData

class CoreDataStack {
    
    let managedObjectModelName: String
    
    private lazy var managedObjectModel: NSManagedObjectModel = {
        let modelURL =
            NSBundle.mainBundle().URLForResource(self.managedObjectModelName,
                                                 withExtension: "momd")!
        return NSManagedObjectModel(contentsOfURL: modelURL)!
    }()
    
    
    required init(modelName: String) {
        managedObjectModelName = modelName
    }
    
    private var applicationDocumentsDirectory: NSURL = {
        let urls = NSFileManager.defaultManager().URLsForDirectory(.DocumentDirectory,
                                                                   inDomains: .UserDomainMask)
        return urls.first!
    }()
    
    private lazy var persistentStoreCoordinator: NSPersistentStoreCoordinator = {
        
        var coordinator =
            NSPersistentStoreCoordinator(managedObjectModel: self.managedObjectModel)
        
        let pathComponent = "\(self.managedObjectModelName).sqlite"
        let url =
            self.applicationDocumentsDirectory.URLByAppendingPathComponent(pathComponent)
        
        let store = try! coordinator.addPersistentStoreWithType(NSSQLiteStoreType,
                                                                configuration: nil,
                                                                URL: url,
                                                                options: nil)
        
        return coordinator
    }()
    
    lazy var mainQueueContext: NSManagedObjectContext = {
        
        let moc = NSManagedObjectContext(concurrencyType: .MainQueueConcurrencyType)
        moc.persistentStoreCoordinator = self.persistentStoreCoordinator
        moc.name = "Main Queue Context (UI Context)"
        
        return moc
    }()

    
    
}