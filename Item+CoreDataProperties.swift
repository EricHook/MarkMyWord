//
//  Item+CoreDataProperties.swift
//  MarkMyWord
//
//  Created by Eric Hook on 12/4/15.
//  Copyright © 2015 Hook Studios. All rights reserved.
//
//  Choose "Create NSManagedObject Subclass…" from the Core Data editor menu
//  to delete and recreate this implementation file for your updated model.
//

import Foundation
import CoreData

extension Item {

    @NSManaged var quantity: NSNumber?
    @NSManaged var photoData: NSData?
    @NSManaged var name: String?
    @NSManaged var listed: NSNumber?
    @NSManaged var collected: NSNumber?

}
