//
//  Vertex.swift
//  swiftyDawg
//
//  Created by hook on 12/18/15.
//  Copyright © 2015 hookstudios. All rights reserved.
//
//  Word Set which needs path and pre-computed set count
//

import Foundation

class WordSet {
    var wordSet : NSMutableSet
    var filePath : String
    
    init(filePath: String, precomputedSize: Int) {
        self.wordSet = NSMutableSet(capacity: precomputedSize)
        self.filePath = filePath
    }
    
    func contains(fragment : String) -> Bool {
        return self.wordSet.containsObject(fragment)
    }
    
    func count() -> Int {
        return wordSet.count
    }
    
    func load() throws {
        do {
            let readString = try NSString(contentsOfFile: filePath, encoding: NSUTF8StringEncoding) as String
            readString.enumerateLines { (line, stop) -> () in
                self.wordSet.addObject(line)
            }
        } catch {
            throw WordSetError.ErrorLoadingFile
        }
    }
}