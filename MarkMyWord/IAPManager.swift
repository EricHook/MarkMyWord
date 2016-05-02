////
////  IAPManager.swift
////  MarkMyWord
////
////  Created by Eric Hook on 4/16/16.
////  Copyright © 2016 Hook Studios. All rights reserved.
////
//
//import UIKit
//import StoreKit
//
//protocol IAPManagerDelegate {
//    func managerDidRestorePurchases()
//}
//
//class IAPManager: NSObject, SKProductsRequestDelegate, SKPaymentTransactionObserver, SKRequestDelegate {
//    
//    static let sharedInstance = IAPManager()
//    
//    var request:SKProductsRequest!
//    var products:NSArray!
//    
//    var delegate:IAPManagerDelegate?
//
//    //Load product identifiers for store usage
//    func setupInAppPurchases(){
//        self.validateProductIdentifiers(self.getProductIdentifiersFromMainBundle())
//        
//        SKPaymentQueue.defaultQueue().addTransactionObserver(self)
//    }
//    
//    
//    //Get product identifiers
//    func getProductIdentifiersFromMainBundle() -> NSArray {
//        var identifiers = NSArray()
//        if let url = NSBundle.mainBundle().URLForResource("iap_product_ids", withExtension: "plist"){
//            identifiers = NSArray(contentsOfURL: url)!
//        }
//        
//        return identifiers
//    }
//    
//    
//    //Retrieve product information
//    func validateProductIdentifiers(identifiers:NSArray) {
//        if debugMode == true { print("IAPManager validateProductIdentifiers(identifiers:NSArray) deluxeVersionPurchased? \(deluxeVersionPurchased)") }
//        let productIdentifiers = NSSet(array: identifiers as [AnyObject])
//        let productRequest = SKProductsRequest(productIdentifiers: productIdentifiers as! Set<String>)
//        self.request = productRequest
//        productRequest.delegate = self
//        productRequest.start()
//    }
//    
//    
//    func createPaymentRequestForProduct(product:SKProduct){
//        let payment = SKMutablePayment(product: product)
//        payment.quantity = 1
//        
//        SKPaymentQueue.defaultQueue().addPayment(payment)
//    }
//    
//    
//    func verifyReceipt(transaction:SKPaymentTransaction?){
//        if debugMode == true { print("IAPManager verifyReceipt(transaction:SKPaymentTransaction?) deluxeVersionPurchased? \(deluxeVersionPurchased)") }
//        
//        var json : NSDictionary?
//        
//        let receiptURL = NSBundle.mainBundle().appStoreReceiptURL!
//        
//        if let receipt = NSData(contentsOfURL: receiptURL){
//            //Receipt exists
//            let requestContents = ["receipt-data" : receipt.base64EncodedStringWithOptions(NSDataBase64EncodingOptions(rawValue: 0)), "password" : "326200e390c84cda8f7fb53750b72e05"]
//            
//            //Perform request
//            do {
//                var requestData = try NSJSONSerialization.dataWithJSONObject(requestContents, options: NSJSONWritingOptions(rawValue: 0))
//                
//                ///////////////////////////////////////////////////
//                
//                //Build URL Request
//                
//                // production URL
//                var storeURL = NSURL(string: "https://buy.itunes.apple.com/verifyReceipt")
//                
////                //sandbox URL
////                var storeURL = NSURL(string: "https://sandbox.itunes.apple.com/verifyReceipt")
//                
//                ///////////////////////////////////////////////////
//                
//                var request = NSMutableURLRequest(URL: storeURL!)
//                request.HTTPMethod = "Post"
//                request.HTTPBody = requestData
//                
//                var session = NSURLSession.sharedSession()
//
//                ////////////////
//
//                
//                var taskStore = session.dataTaskWithRequest(request, completionHandler: { (responseData:NSData?, response:NSURLResponse?, error:NSError?) -> Void in
//
//                    do {
//                        json = try NSJSONSerialization.JSONObjectWithData(responseData!, options: .MutableLeaves) as! NSDictionary
//                        print("IAPManager print(json)")
//                        print(json)
//
//                        //////////////////////////////////////// ERROR getting to Apple store, use sandbox instead
//
//                        if (json?.objectForKey("status") as! NSNumber) != 0 {
//                            print("IAPManager json.objectForKey( status ) as! NSNumber) != 0")
//                            
//                            storeURL = NSURL(string: "https://sandbox.itunes.apple.com/verifyReceipt")
//                            request = NSMutableURLRequest(URL: storeURL!)
//                            request.HTTPMethod = "Post"
//                            request.HTTPBody = requestData
//                            requestData = try NSJSONSerialization.dataWithJSONObject(requestContents, options: NSJSONWritingOptions(rawValue: 0))
//                            
//                            
//                            var taskStoreSandbox = session.dataTaskWithRequest(request, completionHandler: { (responseData:NSData?, response:NSURLResponse?, error:NSError?) -> Void in
//                                
//                                do {
//                                    json = try NSJSONSerialization.JSONObjectWithData(responseData!, options: .MutableLeaves) as! NSDictionary
//                                    print("IAPManager print(json)")
//                                    print(json)
//                                    
//                                    
//                                    
//                                    //////////////////////////////////////// ERROR getting to Apple store, use sandbox instead
//                                    
//                                    
//                                    if (json?.objectForKey("status") as! NSNumber) != 0 {
//                                        print("IAPManager json.objectForKey( status ) as! NSNumber) != 0 2")
//                                        
//                                        storeURL = NSURL(string: "https://sandbox.itunes.apple.com/verifyReceipt")
//                                        request = NSMutableURLRequest(URL: storeURL!)
//                                        request.HTTPMethod = "Post"
//                                        request.HTTPBody = requestData
//                                        requestData = try NSJSONSerialization.dataWithJSONObject(requestContents, options: NSJSONWritingOptions(rawValue: 0))
//                                        
//                                        
//                                        
//                                        
//                                        //                            var taskStoreSandbox = session.dataTaskWithRequest(request, completionHandler: { (responseData:NSData?, response:NSURLResponse?, error:NSError?) -> Void in
//                                        //
//                                        //                                do {
//                                        //
//                                        //
//                                        //                                    json = try NSJSONSerialization.JSONObjectWithData(responseData!, options: .MutableLeaves) as! NSDictionary
//                                        //                                    print("IAPManager print(json)")
//                                        //                                    print(json)
//                                        //
//                                        //
//                                        //                                    // hack to purchase if appStore error
//                                        //                                    //deluxeVersionPurchased = 1
//                                        //
//                                        //                                })
//                                        //                            }
//                                        //                        }
//                                        
//                                        //                                } catch {
//                                        //                                    print(error)
//                                        //                                }
//  
//                                        
//                                    }
//                                    
//                                    
//                                    
//                                    
//                                    //                            let taskSandbox = session.dataTaskWithRequest(request, completionHandler: { (responseData:NSData?, response:NSURLResponse?, error:NSError?) -> Void in
//                                    //                                do {
//                                    //                                    json = try NSJSONSerialization.JSONObjectWithData(responseData!, options: .MutableLeaves) as! NSDictionary
//                                    //                                    print("IAPManager print(json)")
//                                    //                                    print(json)
//                                    //
//                                    //
//                                    //                                } catch {
//                                    //                                    print("IAPManager catch error")
//                                    //                                    print(error)
//                                    //                                }
//                                    //                            })
//                                    
//                                    
//                                    
//                                    ////////////////////////////////
//                                    
//                                    
//                                    
//                                    
//                                    if (json?.objectForKey("status") as! NSNumber) == 0 {
//                                        //
//                                        
//                                        if let latest_receipt = json?["latest_receipt_info 2"]{
//                                            self.validatePurchaseArray(latest_receipt as! NSArray)
//                                        } else {
//                                            let receipt_dict = json?["receipt"] as! NSDictionary
//                                            if let purchases = receipt_dict["in_app"] as? NSArray{
//                                                self.validatePurchaseArray(purchases)
//                                            }
//                                        }
//                                        
//                                        
//                                        if transaction != nil {
//                                            SKPaymentQueue.defaultQueue().finishTransaction(transaction!)
//                                            
//                                            if debugMode == true {
//                                                print("IAPManager dispatch_sync( dispatch_get_main_queue  )  1  deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                                            }
//                                            
//                                            deluxeVersionPurchased = 1
//                                            
//                                            if debugMode == true {print("IAPManager if transaction != nil verifyReceipt(transaction:SKPaymentTransaction?) deluxeVersionPurchased? \(deluxeVersionPurchased)") }
//                                            
//                                            
//                                        }
//                                        
//                                        dispatch_sync(dispatch_get_main_queue(), { () -> Void in
//                                            self.delegate?.managerDidRestorePurchases()
//                                            
//                                            if debugMode == true {
//                                                print("IAPManager dispatch_sync( dispatch_get_main_queue  ) 2 deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                                            }
//                                            
//                                            //deluxeVersionPurchased = 1
//                                            //gameViewController.updateUIDeluxeVersion()
//                                            
//                                            
//                                        })
//                                        
//                                    } else {
//                                        //Debug the receipt
//                                        try print(json?.objectForKey("IAPManager status") as? NSNumber)
//                                    }
//                                    
//                                    
//                                    
//                                } catch {
//                                    print("IAPManager catch error")
//                                    print(error)
//                                }
//                            })
//                            
//                            
//                            taskStoreSandbox.resume()
//                            
////                            var taskStoreSandbox = session.dataTaskWithRequest(request, completionHandler: { (responseData:NSData?, response:NSURLResponse?, error:NSError?) -> Void in
////                            
////                                do {
////                                    
////                                    
////                                    json = try NSJSONSerialization.JSONObjectWithData(responseData!, options: .MutableLeaves) as! NSDictionary
////                                    print("IAPManager print(json)")
////                                    print(json)
////                                    
////                                    
////                                    // hack to purchase if appStore error
////                                    //deluxeVersionPurchased = 1
////                                    
////                                })
////                            }
////                        }
//
////                                } catch {
////                                    print(error)
////                                }
//                            
//                            
//                            
//                            
//                            
//                            
//                            
//                            
//                            
//                            
//                            
//                            }
//
//                        
//                            
//                            
////                            let taskSandbox = session.dataTaskWithRequest(request, completionHandler: { (responseData:NSData?, response:NSURLResponse?, error:NSError?) -> Void in
////                                do {
////                                    json = try NSJSONSerialization.JSONObjectWithData(responseData!, options: .MutableLeaves) as! NSDictionary
////                                    print("IAPManager print(json)")
////                                    print(json)
////     
////                            
////                                } catch {
////                                    print("IAPManager catch error")
////                                    print(error)
////                                }
////                            })
//
//                       
//
//                        ////////////////////////////////
//                
//                        
//
//                        
//                        if (json?.objectForKey("status") as! NSNumber) == 0 {
//                            //
//                            
//                            if let latest_receipt = json?["latest_receipt_info"]{
//                                self.validatePurchaseArray(latest_receipt as! NSArray)
//                            } else {
//                                let receipt_dict = json?["receipt"] as! NSDictionary
//                                if let purchases = receipt_dict["in_app"] as? NSArray{
//                                    self.validatePurchaseArray(purchases)
//                                }
//                            }
//
//                            
//                            if transaction != nil {
//                                SKPaymentQueue.defaultQueue().finishTransaction(transaction!)
//
//                                if debugMode == true {
//                                    print("IAPManager dispatch_sync( dispatch_get_main_queue  )  1  deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                                }
//                                
//                                deluxeVersionPurchased = 1
//                                
//                                if debugMode == true {print("IAPManager if transaction != nil verifyReceipt(transaction:SKPaymentTransaction?) deluxeVersionPurchased? \(deluxeVersionPurchased)") }
//
//     
//                            }
//                            
//                            dispatch_sync(dispatch_get_main_queue(), { () -> Void in
//                                self.delegate?.managerDidRestorePurchases()
//                                
//                                if debugMode == true {
//                                    print("IAPManager dispatch_sync( dispatch_get_main_queue  ) 2 deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                                }
//                                    
//                                //deluxeVersionPurchased = 1
//                                //gameViewController.updateUIDeluxeVersion()
//                                
//                                
//                            })
//                            
//                        } else {
//                            //Debug the receipt
//                            try print(json?.objectForKey("IAPManager status") as? NSNumber)
//                        }
//                        
//                    } catch {
//                         print("IAPManager catch error")
//                        print(error)
//                    }
//                })
//
//                taskStore.resume()
//                    
//           
//                
//            } catch {
//                print(error)
//            }
//                
//  
//            
//        } else {
//            //Receipt does not exist
//            //deluxeVersionPurchased = 0
//            print("IAPManager verifyReceipt >> No Receipt deluxeVersionPurchased? \(deluxeVersionPurchased)")
//        }
//    }
//    
//    
//    func validatePurchaseArray(purchases:NSArray){
//        if debugMode == true { print("IAPManager validatePurchaseArray(purchases:NSArray) deluxeVersionPurchased? \(deluxeVersionPurchased)") }
//        for purchase in purchases as! [NSDictionary]{
//            
//            
////            deluxeVersionPurchased = 1
////            gameViewController.updateUIDeluxeVersion()
//            
//            
//            if let expires_date_ms_string = purchase["expires_date_ms"] as? String {
//                let expires_date_ms = NSNumberFormatter().numberFromString(expires_date_ms_string)
//                if self.isDateExpired(expires_date_ms as! Double){
//                    //
//                    print("IAPManager Expired subscription")
//                    self.lockPurchasedFunctionalityForProductIdentifier(purchase["product_id"] as! String)
//                } else {
//                    print("IAPManager Active subscription")
//                    self.unlockPurchasedFunctionalityForProductIdentifier(purchase["product_id"] as! String)
//                }
//                
//            } else {
//                self.unlockPurchasedFunctionalityForProductIdentifier(purchase["product_id"] as! String)
//            }
//        }
//    }
//    
//    
//    func unlockPurchasedFunctionalityForProductIdentifier(productIdentifier:String){
//        
//        deluxeVersionPurchased = 1
//        
//        if debugMode == true {
//            print("IAPManager unlockPurchasedFunctionalityForProductIdentifier(productIdentifier:String) deluxeVersionPurchased? \(deluxeVersionPurchased)")
//        }
//        
//        NSUserDefaults.standardUserDefaults().setBool(true, forKey: productIdentifier)
//        NSUserDefaults.standardUserDefaults().synchronize()
//        
//        UIApplication.sharedApplication().networkActivityIndicatorVisible = false
//
//        gameViewController.updateUIDeluxeVersion()
//    }
//    
//    
//    func lockPurchasedFunctionalityForProductIdentifier(productIdentifier:String){
//        
//        NSUserDefaults.standardUserDefaults().setBool(false, forKey: productIdentifier)
//        NSUserDefaults.standardUserDefaults().synchronize()
//        
//        UIApplication.sharedApplication().networkActivityIndicatorVisible = false
//    }
//    
//    
//    func isDateExpired(expires_date:Double) -> Bool{
//        var isExpired:Bool = false
//        let currentDate = (NSDate().timeIntervalSince1970 * 1000) as NSTimeInterval
//        
//        if currentDate > expires_date{
//            isExpired = true
//        }
//        
//        return isExpired
//    }
//    
//    
//    //MARK: SKProductsRequestDelegate
//    func productsRequest(request: SKProductsRequest, didReceiveResponse response: SKProductsResponse) {
//        if debugMode == true { print("IAPManager productsRequest >> \(self.products) deluxeVersionPurchased? \(deluxeVersionPurchased)") }
//        self.products = response.products
//    }
//    
//    
//    //MARK: SKPaymentTransactionObserver Delegate Protocol
//    func paymentQueue(queue: SKPaymentQueue, updatedTransactions transactions: [SKPaymentTransaction]) {
//        if debugMode == true { print("IAPManager paymentQueue(queue: SKPaymentQueue, updatedTransactions transactions: [SKPaymentTransaction]) deluxeVersionPurchased? \(deluxeVersionPurchased)") }
//        for transaction in transactions as [SKPaymentTransaction]{
//            switch transaction.transactionState{
//            case .Purchasing:
//                print("IAPManager Purchasing deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                UIApplication.sharedApplication().networkActivityIndicatorVisible = true
//            case .Deferred:
//                print("IAPManager Deferrred deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                UIApplication.sharedApplication().networkActivityIndicatorVisible = false
//            case .Failed:
//                print("IAPManager Failed deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                print(transaction.error?.localizedDescription)
//                UIApplication.sharedApplication().networkActivityIndicatorVisible = false
//                SKPaymentQueue.defaultQueue().finishTransaction(transaction)
//            case.Purchased:
//                print("IAPManager Purchased deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                //
//                self.verifyReceipt(transaction)
//                
//                if debugMode == true {
//                    print("IAPManager func paymentQueue case.Purchased: deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                }
//                
//                //deluxeVersionPurchased = 1 //////////////////////////
//                //gameViewController.updateUIDeluxeVersion()
//                
//            case .Restored:
//                print("IAPManager Restored deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                
//                
//                if debugMode == true {
//                    print("IAPManager func paymentQueue case .Restored: deluxeVersionPurchased? \(deluxeVersionPurchased)")
//                }
//                //deluxeVersionPurchased = 1 //////////////////////////
//                //gameViewController.updateUIDeluxeVersion()
//                
//            }
//        }
//    }
//    
//    
//    func restorePurchases(){
//        if debugMode == true { print("IAPManager restorePurchases() deluxeVersionPurchased? \(deluxeVersionPurchased)") }
//        let request = SKReceiptRefreshRequest()
//        request.delegate = self
//        request.start()
//        
//    }
//    
//    
//    func requestDidFinish(request: SKRequest) {
//        if debugMode == true { print("IAPManager requestDidFinish(request: SKRequest) deluxeVersionPurchased? \(deluxeVersionPurchased)") }
//        self.verifyReceipt(nil)
//        
//    }
//    
//}
//
