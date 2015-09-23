//
//  ViewController.swift
//  TreiTest
//
//  Created by Eric Hook on 9/6/15.
//  Copyright © 2015 Eric Hook. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        
        
        let words  = ["hello", "hiya", "hell", "jonah", "jolly", "joseph", "jobs"].map{$0.characters}
        
        var store = Trie(words)
        
        store.map(String.init)
        
        
        store
            .contains("hello".characters)
        
        store
            .completions("hel".characters) 
            .map(String.init)
        
        store
            .remove("jonah".characters)
        
        store
            .completions("jo".characters)
            .map(String.init)
        
        //store
        
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

