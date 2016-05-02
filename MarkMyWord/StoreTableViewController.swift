////
////  StoreTableViewController.swift
////  NoteTaker
////
////  Created by Ron Buencamino on 2/3/16.
////  Copyright © 2016 Roux Academy. All rights reserved.
////
//// from Lynda.com training
//
//import UIKit
//import StoreKit
//
//class StoreTableViewController: UIViewController, UITableViewDataSource, UITableViewDelegate, IAPManagerDelegate {
//
//    override func viewDidLoad() {
//        super.viewDidLoad()
//
//        // Uncomment the following line to preserve selection between presentations
//        // self.clearsSelectionOnViewWillAppear = false
//
//        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
//        // self.navigationItem.rightBarButtonItem = self.editButtonItem()
//        
//        IAPManager.sharedInstance.delegate = self
//    }
//
//    override func didReceiveMemoryWarning() {
//        super.didReceiveMemoryWarning()
//        // Dispose of any resources that can be recreated.
//    }
//
//    // MARK: - Table view data source
//
//    func numberOfSectionsInTableView(tableView: UITableView) -> Int {
//        // #warning Incomplete implementation, return the number of sections
//        return 1
//    }
//
//    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
//        // #warning Incomplete implementation, return the number of rows
//        return IAPManager.sharedInstance.products.count
//    }
//
//    
//    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
//        let cell = UITableViewCell(style: .Subtitle, reuseIdentifier: "reuseIdentifier")
//
//        // Configure the cell...
//        
//        let product = IAPManager.sharedInstance.products.objectAtIndex(indexPath.row) as! SKProduct
//        
//        cell.textLabel!.text = product.localizedTitle
//        cell.detailTextLabel!.text = product.localizedDescription
//
//        return cell
//    }
//    
//    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
//        tableView.deselectRowAtIndexPath(indexPath, animated: true)
//        
//        //3 - call payment request function with selected SKProduct
//        IAPManager.sharedInstance.createPaymentRequestForProduct(IAPManager.sharedInstance.products.objectAtIndex(indexPath.row) as! SKProduct)
//    }
//    
//    //MARK: Helper
//    @IBAction func closeStoreView(){
//        self.presentingViewController?.dismissViewControllerAnimated(true, completion: nil)
//        
//
//    }
//    
//    @IBAction func restorePurchases(){
//        IAPManager.sharedInstance.restorePurchases()
//    }
//    
//    func managerDidRestorePurchases() {
//        
//        let alertController = UIAlertController(title: "In-App Purchase", message: "Your purchases have been restored", preferredStyle: .Alert)
//        let okAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
//        alertController.addAction(okAction)
//        
//        
//        self.presentViewController(alertController, animated: true, completion: nil)
//  
//    }
//    
//
//    /*
//    // Override to support conditional editing of the table view.
//    override func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
//        // Return false if you do not want the specified item to be editable.
//        return true
//    }
//    */
//
//    /*
//    // Override to support editing the table view.
//    override func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
//        if editingStyle == .Delete {
//            // Delete the row from the data source
//            tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: .Fade)
//        } else if editingStyle == .Insert {
//            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
//        }    
//    }
//    */
//
//    /*
//    // Override to support rearranging the table view.
//    override func tableView(tableView: UITableView, moveRowAtIndexPath fromIndexPath: NSIndexPath, toIndexPath: NSIndexPath) {
//
//    }
//    */
//
//    /*
//    // Override to support conditional rearranging of the table view.
//    override func tableView(tableView: UITableView, canMoveRowAtIndexPath indexPath: NSIndexPath) -> Bool {
//        // Return false if you do not want the item to be re-orderable.
//        return true
//    }
//    */
//
//    /*
//    // MARK: - Navigation
//
//    // In a storyboard-based application, you will often want to do a little preparation before navigation
//    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
//        // Get the new view controller using segue.destinationViewController.
//        // Pass the selected object to the new view controller.
//    }
//    */
//
//}
