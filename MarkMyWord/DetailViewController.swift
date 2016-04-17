//
//  DetailViewController.swift
//  NoteTaker
//
//  Created by Ron Buencamino on 10/5/15.
//  Copyright © 2015 Roux Academy. All rights reserved.
//

import UIKit
import CoreData

class DetailViewController: UIViewController, UITextFieldDelegate, UITextViewDelegate {
   
    @IBOutlet weak var titleField: UITextField!
    @IBOutlet weak var createdOnLabel: UILabel!
    @IBOutlet weak var noteDetailTextView: UITextView!
    @IBOutlet weak var saveButton: UIButton!
    @IBOutlet weak var attachmentButton: UIButton!
    
    var detailItem: AnyObject? {
        didSet {
            self.configureView()
        }
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        
        let appDelegate = UIApplication.sharedApplication().delegate as! AppDelegate
        
        if appDelegate.canPurchase {
            let photoNoteButton = UIBarButtonItem(image: UIImage(named: "photoNotesImage"), style: .Plain, target: self, action: "photoNotesAction")
            self.navigationItem.rightBarButtonItem = photoNoteButton
        }
        
        // Configure View
        self.configureView()
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    //MARK: Photo Note Functionality
    func photoNotesAction(){
        if NSUserDefaults.standardUserDefaults().boolForKey("com.NoteTaker.photonotes"){
            print("Active")
        } else {
            let alertController = UIAlertController(title: "Photo Notes", message: "Did you know that you can attach pictures to the notes you create?", preferredStyle: .Alert)
            let learnAction = UIAlertAction(title: "Learn More", style: .Default) { (action) -> Void in
                print("action")
                let vc = self.storyboard?.instantiateViewControllerWithIdentifier("StoreTableView") as! StoreTableViewController
                self.presentViewController(vc, animated: true, completion: nil)
            }
            let cancelAction = UIAlertAction(title: "Not right now", style: .Default, handler: nil)
            alertController.addAction(cancelAction)
            alertController.addAction(learnAction)
            self.presentViewController(alertController, animated: true, completion: nil)
        }
        
        
    }
    
    @IBAction func attachmentAction(){
        print("action")
    }
    
    
    //MARK: View Functions
    
    func configureView(){
        if let detail = self.detailItem {
            if let noteTitle = self.titleField {
                noteTitle.text = detail.valueForKey("title")!.description
            }
            if let createdOn = self.createdOnLabel {
                let formattedDate = GameManager.sharedInstance.formatDateToString(detail.valueForKey("timeStamp") as! NSDate)
                createdOn.text = "Created On: " + formattedDate
            }
            if let noteDetail = self.noteDetailTextView {
                noteDetail.text = detail.valueForKey("detail")!.description
            }
        } else {
            if let noteTitle = self.titleField {
                noteTitle.text = ""
            }
            if let createdOn = self.createdOnLabel {
                createdOn.text = "Created On: " + GameManager.sharedInstance.formatDateToString(NSDate())
            }
            if let noteDetail = self.noteDetailTextView {
                noteDetail.text = "Note Detail"
                noteDetail.textColor = UIColor.lightGrayColor()
            }
        }
    }
    
    func saveNote(){
        let title = self.titleField.text
        let detail = self.noteDetailTextView.text
        
        GameManager.sharedInstance.insertNewObject(title!, detail: detail)
    }
    
    func updateNote(){
        if let detail = self.detailItem {
            detail.setValue(self.titleField!.text, forKey: "title")
            detail.setValue(self.noteDetailTextView!.text, forKey: "detail")
            
            do {
                try GameManager.sharedInstance.managedObjectContext.save()
            } catch {
                abort()
            }
        }
    }
    
    @IBAction func saveButtonPressed(sender: AnyObject) {
        if let _ = self.detailItem {
            self.updateNote()
        } else {
            self.saveNote()
        }
    }

    // MARK: - UITextField
    
    func textField(textField: UITextField, shouldChangeCharactersInRange range: NSRange, replacementString string: String) -> Bool {
        
        self.saveButton.tintColor = UIColor.blueColor()
        self.saveButton.enabled = true
        
        return true
    }
    
    // MARK: - UITextView
    
    func textView(textView: UITextView, shouldChangeTextInRange range: NSRange, replacementText text: String) -> Bool {
        self.saveButton.tintColor = UIColor.blueColor()
        self.saveButton.enabled = true
        
        return true
    }
    
    func textViewDidBeginEditing(textView: UITextView) {
        if (self.detailItem == nil) {
            textView.text = ""
            textView.textColor = UIColor.blackColor()
        }
    }

}

