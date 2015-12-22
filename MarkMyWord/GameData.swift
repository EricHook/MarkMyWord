//
//  GameData.swift
//  Anagrams
//


import Foundation

class GameData {
  //store the user's game achievement
  var points : Int = 0 {
    didSet {
      //custom setter - keep the score positive
      points = max(points, 0)
    }
  }
}
