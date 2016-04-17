//
//  IAPManager.swift
//  MarkMyWord
//
//  Created by Eric Hook on 4/16/16.
//  Copyright © 2016 Hook Studios. All rights reserved.
//

import UIKit
import StoreKit

protocol IAPManagerDelegate {
    func managerDidRestorePurchases()
}

class IAPManager: NSObject, SKProductsRequestDelegate, SKPaymentTransactionObserver, SKRequestDelegate {
    
    static let sharedInstance = IAPManager()
    
    var request:SKProductsRequest!
    var products:NSArray!
    
    var delegate:IAPManagerDelegate?

    //Load product identifiers for store usage
    func setupInAppPurchases(){
        self.validateProductIdentifiers(self.getProductIdentifiersFromMainBundle())
        
        SKPaymentQueue.defaultQueue().addTransactionObserver(self)
    }
    
    
    //Get product identifiers
    func getProductIdentifiersFromMainBundle() -> NSArray {
        var identifiers = NSArray()
        if let url = NSBundle.mainBundle().URLForResource("iap_product_ids", withExtension: "plist"){
            identifiers = NSArray(contentsOfURL: url)!
        }
        
        return identifiers
    }
    
    
    //Retrieve product information
    func validateProductIdentifiers(identifiers:NSArray) {
        let productIdentifiers = NSSet(array: identifiers as [AnyObject])
        let productRequest = SKProductsRequest(productIdentifiers: productIdentifiers as! Set<String>)
        self.request = productRequest
        productRequest.delegate = self
        productRequest.start()
    }
    
    
    func createPaymentRequestForProduct(product:SKProduct){
        let payment = SKMutablePayment(product: product)
        payment.quantity = 1
        
        SKPaymentQueue.defaultQueue().addPayment(payment)
    }
    
    
    func verifyReceipt(transaction:SKPaymentTransaction?){
        let receiptURL = NSBundle.mainBundle().appStoreReceiptURL!
        if let receipt = NSData(contentsOfURL: receiptURL){
            //Receipt exists
            let requestContents = ["receipt-data" : receipt.base64EncodedStringWithOptions(NSDataBase64EncodingOptions(rawValue: 0)), "password" : "tgbyhn"] // "password" : "326200e390c84cda8f7fb53750b72e05"
            
            //Perform request
            do {
                let requestData = try NSJSONSerialization.dataWithJSONObject(requestContents, options: NSJSONWritingOptions(rawValue: 0))
                
                ///////////////////////////////////////////////////
                
                //Build URL Request
                
//                // production URL
//                let storeURL = NSURL(string: "https:/buy.itunes.apple.com/verifyReceipt")
                
                //sandbox URL
                let storeURL = NSURL(string: "https:/sandbox.itunes.apple.com/verifyReceipt")
                
                ///////////////////////////////////////////////////
                
                let request = NSMutableURLRequest(URL: storeURL!)
                request.HTTPMethod = "Post"
                request.HTTPBody = requestData
                
                let session = NSURLSession.sharedSession()
                let task = session.dataTaskWithRequest(request, completionHandler: { (responseData:NSData?, response:NSURLResponse?, error:NSError?) -> Void in
                    //
                    
                    do {
                        let json = try NSJSONSerialization.JSONObjectWithData(responseData!, options: .MutableLeaves) as! NSDictionary
                        
                        print(json)
                        
                        if (json.objectForKey("status") as! NSNumber) == 0 {
                            //
                            
                            if let latest_receipt = json["latest_receipt_info"]{
                                self.validatePurchaseArray(latest_receipt as! NSArray)
                            } else {
                                let receipt_dict = json["receipt"] as! NSDictionary
                                if let purchases = receipt_dict["in_app"] as? NSArray{
                                    self.validatePurchaseArray(purchases)
                                }
                            }

                            if transaction != nil {
                                SKPaymentQueue.defaultQueue().finishTransaction(transaction!)

                                
                                deluxeVersionPurchased = true
                                gameViewController.updateUIDeluxeVersion()
                                
                                
                            }
                            
                            dispatch_sync(dispatch_get_main_queue(), { () -> Void in
                                self.delegate?.managerDidRestorePurchases()
                                
                                
                                deluxeVersionPurchased = true
                                gameViewController.updateUIDeluxeVersion()
                                
                                
                            })
                            
                        } else {
                            //Debug the receipt
                            print(json.objectForKey("status") as! NSNumber)
                        }
                        
                    } catch {
                        print(error)
                    }
                })
                
                task.resume()
                
            } catch {
                print(error)
            }
            
        } else {
            //Receipt does not exist
            print("verifyReceipt >> No Receipt")
            deluxeVersionPurchased = false
        }
    }
    
    
    func validatePurchaseArray(purchases:NSArray){
        
        for purchase in purchases as! [NSDictionary]{
            
            
//            deluxeVersionPurchased = true
//            gameViewController.updateUIDeluxeVersion()
            
            
            if let expires_date_ms_string = purchase["expires_date_ms"] as? String {
                let expires_date_ms = NSNumberFormatter().numberFromString(expires_date_ms_string)
                if self.isDateExpired(expires_date_ms as! Double){
                    //
                    print("Expired subscription")
                    self.lockPurchasedFunctionalityForProductIdentifier(purchase["product_id"] as! String)
                } else {
                    print("Active subscription")
                    self.unlockPurchasedFunctionalityForProductIdentifier(purchase["product_id"] as! String)
                }
                
            } else {
                self.unlockPurchasedFunctionalityForProductIdentifier(purchase["product_id"] as! String)
            }
        }
    }
    
    
    func unlockPurchasedFunctionalityForProductIdentifier(productIdentifier:String){
        
        NSUserDefaults.standardUserDefaults().setBool(true, forKey: productIdentifier)
        NSUserDefaults.standardUserDefaults().synchronize()
        
        UIApplication.sharedApplication().networkActivityIndicatorVisible = false
        
        deluxeVersionPurchased = true
        gameViewController.updateUIDeluxeVersion()
    }
    
    
    func lockPurchasedFunctionalityForProductIdentifier(productIdentifier:String){
        
        NSUserDefaults.standardUserDefaults().setBool(false, forKey: productIdentifier)
        NSUserDefaults.standardUserDefaults().synchronize()
        
        UIApplication.sharedApplication().networkActivityIndicatorVisible = false
    }
    
    
    func isDateExpired(expires_date:Double) -> Bool{
        var isExpired:Bool = false
        let currentDate = (NSDate().timeIntervalSince1970 * 1000) as NSTimeInterval
        
        if currentDate > expires_date{
            isExpired = true
        }
        
        return isExpired
    }
    
    
    //MARK: SKProductsRequestDelegate
    func productsRequest(request: SKProductsRequest, didReceiveResponse response: SKProductsResponse) {
        //
        self.products = response.products
        print("productsRequest >> \(self.products) ")
    }
    
    
    //MARK: SKPaymentTransactionObserver Delegate Protocol
    func paymentQueue(queue: SKPaymentQueue, updatedTransactions transactions: [SKPaymentTransaction]) {
        //
        for transaction in transactions as [SKPaymentTransaction]{
            switch transaction.transactionState{
            case .Purchasing:
                print("Purchasing")
                UIApplication.sharedApplication().networkActivityIndicatorVisible = true
            case .Deferred:
                print("Deferrred")
                UIApplication.sharedApplication().networkActivityIndicatorVisible = false
            case .Failed:
                print("Failed")
                print(transaction.error?.localizedDescription)
                UIApplication.sharedApplication().networkActivityIndicatorVisible = false
                SKPaymentQueue.defaultQueue().finishTransaction(transaction)
            case.Purchased:
                print("Purchased")
                //
                self.verifyReceipt(transaction)
                
//                deluxeVersionPurchased = true //////////////////////////
//                gameViewController.updateUIDeluxeVersion()
                
            case .Restored:
                print("Restored")
                
//                deluxeVersionPurchased = true //////////////////////////
//                gameViewController.updateUIDeluxeVersion()
                
            }
        }
    }
    
    
    func restorePurchases(){
        let request = SKReceiptRefreshRequest()
        request.delegate = self
        request.start()
    }
    
    
    func requestDidFinish(request: SKRequest) {
        self.verifyReceipt(nil)
    }
    
}

