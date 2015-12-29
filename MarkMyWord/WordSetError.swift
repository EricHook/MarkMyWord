//
//  WordSetError.swift
//  MarkMyWord
//
//  Created by hook on 12/29/15.
//  Copyright © 2015 Hook Studios. All rights reserved.
//

import Foundation

enum WordSetError : ErrorType {
    case ErrorLoadingFile
}

extension WordSetError : CustomStringConvertible {
    var description: String {
        switch self {
        case .ErrorLoadingFile:
            return "Error has occurred when loading dictionary file"
        }
    }
}
