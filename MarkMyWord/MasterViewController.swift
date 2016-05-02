////
////  MasterViewController.swift
////  NoteTaker
////
////  Created by Ron Buencamino on 10/5/15.
////  Copyright © 2015 Roux Academy. All rights reserved.
////
//
//import UIKit
//import CoreData
//
//class MasterViewController: UITableViewController, NSFetchedResultsControllerDelegate {
//
//    var detailViewController: DetailViewController? = nil
//    var resultsController = GameManager.sharedInstance.fetchedResultsController
//    
//
//
//    override func viewDidLoad() {
//        super.viewDidLoad()
//        // Do any additional setup after loading the view, typically from a nib.
//        self.navigationItem.leftBarButtonItem = self.editButtonItem()
//        
//        let addButton = UIBarButtonItem(barButtonSystemItem: .Add, target: self, action: "addNewNote")
//        let settingsButton = UIBarButtonItem(image: UIImage(named: "settingsImage"), style: .Plain, target: self, action: "showSettingsMenu")
//        self.navigationItem.rightBarButtonItems = [addButton, settingsButton]
//        if let split = self.splitViewController {
//            let controllers = split.viewControllers
//            self.detailViewController = (controllers[controllers.count-1] as! UINavigationController).topViewController as? DetailViewController
//        }
//        
//        resultsController.delegate = self
//    }
//
//    override func viewWillAppear(animated: Bool) {
//        self.clearsSelectionOnViewWillAppear = self.splitViewController!.collapsed
//        super.viewWillAppear(animated)
//        self.tableView.reloadData()
//    }
//
//    override func didReceiveMemoryWarning() {
//        super.didReceiveMemoryWarning()
//        // Dispose of any resources that can be recreated.
//    }
//    
//    //Mark: - Settings
//    func showSettingsMenu(){
//        let vc = self.storyboard?.instantiateViewControllerWithIdentifier("SettingsTableView") as! SettingsTableViewController
//        self.presentViewController(vc, animated: true, completion: nil)
//    }
//
//    // MARK: - Segues
//
//    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
//        if segue.identifier == "showDetail" {
//            if let indexPath = self.tableView.indexPathForSelectedRow {
//            let object = resultsController.objectAtIndexPath(indexPath)
//                let controller = (segue.destinationViewController as! UINavigationController).topViewController as! DetailViewController
//                controller.detailItem = object
//                controller.navigationItem.leftBarButtonItem = self.splitViewController?.displayModeButtonItem()
//                controller.navigationItem.leftItemsSupplementBackButton = true
//            }
//        }
//    }
//
//    // MARK: - Table View
//
//    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
//        return resultsController.sections?.count ?? 0
//    }
//
//    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
//        let sectionInfo = resultsController.sections![section]
//        return sectionInfo.numberOfObjects
//    }
//
//    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
//        let cell = tableView.dequeueReusableCellWithIdentifier("Cell", forIndexPath: indexPath)
//        self.configureCell(cell, atIndexPath: indexPath)
//        return cell
//    }
//
//    override func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
//        // Return false if you do not want the specified item to be editable.
//        return true
//    }
//
//    override func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
//        if editingStyle == .Delete {
//            let context = resultsController.managedObjectContext
//            context.deleteObject(resultsController.objectAtIndexPath(indexPath) as! NSManagedObject)
//                
//            do {
//                try context.save()
//            } catch {
//                // Replace this implementation with code to handle the error appropriately.
//                // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
//                //print("Unresolved error \(error), \(error.userInfo)")
//                abort()
//            }
//        }
//    }
//
//    func configureCell(cell: UITableViewCell, atIndexPath indexPath: NSIndexPath) {
//        let object = resultsController.objectAtIndexPath(indexPath)
//        cell.textLabel!.text = object.valueForKey("title")!.description
//        cell.detailTextLabel!.text = GameManager.sharedInstance.formatDateToString(object.valueForKey("timeStamp") as! NSDate)
//        
//    }
//
//    
//
//    func controllerWillChangeContent(controller: NSFetchedResultsController) {
//        self.tableView.beginUpdates()
//    }
//
//    func controller(controller: NSFetchedResultsController, didChangeSection sectionInfo: NSFetchedResultsSectionInfo, atIndex sectionIndex: Int, forChangeType type: NSFetchedResultsChangeType) {
//        switch type {
//            case .Insert:
//                self.tableView.insertSections(NSIndexSet(index: sectionIndex), withRowAnimation: .Fade)
//            case .Delete:
//                self.tableView.deleteSections(NSIndexSet(index: sectionIndex), withRowAnimation: .Fade)
//            default:
//                return
//        }
//    }
//
//    func controller(controller: NSFetchedResultsController, didChangeObject anObject: AnyObject, atIndexPath indexPath: NSIndexPath?, forChangeType type: NSFetchedResultsChangeType, newIndexPath: NSIndexPath?) {
//        switch type {
//            case .Insert:
//                tableView.insertRowsAtIndexPaths([newIndexPath!], withRowAnimation: .Fade)
//            case .Delete:
//                tableView.deleteRowsAtIndexPaths([indexPath!], withRowAnimation: .Fade)
//            case .Update:
//                self.configureCell(tableView.cellForRowAtIndexPath(indexPath!)!, atIndexPath: indexPath!)
//            case .Move:
//                tableView.deleteRowsAtIndexPaths([indexPath!], withRowAnimation: .Fade)
//                tableView.insertRowsAtIndexPaths([newIndexPath!], withRowAnimation: .Fade)
//        }
//    }
//
//    func controllerDidChangeContent(controller: NSFetchedResultsController) {
//        self.tableView.endUpdates()
//    }
//    
//    func addNewNote(){
//        let sb = UIStoryboard.init(name: "Main", bundle: nil)
//        let vc = sb.instantiateViewControllerWithIdentifier("DetailView")
//        self.navigationController?.pushViewController(vc, animated: true)
//    }
//
//    /*
//     // Implementing the above methods to update the table view in response to individual changes may have performance implications if a large number of changes are made simultaneously. If this proves to be an issue, you can instead just implement controllerDidChangeContent: which notifies the delegate that all section and object changes have been processed.
//     
//     func controllerDidChangeContent(controller: NSFetchedResultsController) {
//         // In the simplest, most efficient, case, reload the table view.
//         self.tableView.reloadData()
//     }
//     */
//
//}
//
