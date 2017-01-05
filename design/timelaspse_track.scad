rail_radius = 10.1;
rail_distance = 72;
bearing_radius = 8;
bearing_bore_diameter = 3.1;

module rail(tolerance=0) {
  color([0.3, 0.3, 0.3])
  rotate([0, 90, 0]) cylinder(500, r=rail_radius + tolerance, center=true);
}

module rails(tolerance=0) {
  color([0.3, 0.3, 0.3]);
  translate([0, rail_distance / 2 * -1, 0]) rail();
  translate([0, rail_distance / 2, 0]) rail();
}

module foot() {
  union() {
    difference() {
      cube([45, rail_distance + 40, 26], true);
      translate([-235, 0, 0])
        rails(0.2);
      //tripod screw (1/4" - 20)
      translate([-2, -3, 0])
        cylinder(40, d=6.35, center=true, $fn=48);
      
      //timing belt hole
      translate([0, 4.5, -8])
        rotate([0, 90, 0])
          cube([3, 8, 50], center=true);
      
      //screw hole for holding timing belt
      translate([13, -2, -3])
        rotate([0, 90, 0])
          cylinder(20, d=4.17, center=true, $fn=24);
      translate([13, 11, -3])
        rotate([0, 90, 0])
          cylinder(20, d=4.17, center=true, $fn=24);
    }
    translate([0, (rail_distance + 75) / 2, -31])
      rotate([-45, 0, 0])
        cube([45, 65, 12], true);
    translate([0, (rail_distance + 75) / -2, -31])
      rotate([45, 0, 0])
        cube([45, 65, 12], true);
  }
}

module bearing_hole() {
  cube([7, 6, 14], true);
}

module bearing() {
  color([0.6, 0.6, 0.6])
  difference() {
    cylinder(4, r=5, center=true);
    cylinder(5, d=bearing_bore_diameter, center=true, $fn=24);
  }
}

module bearing_mount(supports=false) {
  bearing_height = 4;
  
  difference() {
    union() {
      translate([-1, 5, -1]) cube([20, 4, 14], true);
      translate([-1, -5, -1]) cube([20, 4, 14], true);
      translate([-14, 0, -1]) cube([10, 14, 14], true);
      if (supports) {
        translate([0, -5, 12])
          rotate([0, -45, 0])
            cube([36, 4, 18], true);
        translate([0, 5, 12])
          rotate([0, -45, 0])
            cube([36, 4, 18], true);
      }
    }
    translate([bearing_height, 0, 0]) {
      rotate([90, 0, 0]) cylinder(16, d=bearing_bore_diameter, center=true, $fn=24);
    }
    
    //screw hole (#8 size)
    translate([-14, 0, 0])
      rotate([0, 90, 0])
        cylinder(14, d=4.17, center=true, $fn=24);
  }
  //translate([bearing_height, 0, 0]) rotate([90, 0, 0]) bearing();
}


module sheath() {
  rotate([0, 90, 0]) {
    union() {
      difference() {
        cylinder(92, r=rail_radius + 4, center=true, $fn=50);
        sheath_hole();
        translate([-11.5, 0, 34]) bearing_hole();
        translate([-11.5, 0, -34]) bearing_hole();
      }
      
      translate([-19, 0, 34])
        rotate([180, 0, 0])
          bearing_mount();

      translate([-19, 0, -34]) bearing_mount(true);
    }
  }
}

module sheath_hole(pad_for_sheath=false) {
  cylinder(100, r=rail_radius + 1, center=true, $fn=50);
}

module sheath_holes(include_sheath=false) {
  translate([0, rail_distance / 2, 0]) {
    rotate([0, 90, 0]) 
      sheath_hole(include_sheath);
    if (include_sheath) {
      sheath();
    }
  }
  translate([0, rail_distance / 2 * -1, 0]) {
    rotate([0, 90, 0])
      sheath_hole(include_sheath);
    if (include_sheath) {
      sheath();
    }
  }
  
}



module worm_gear() {
  color([1, .8, .5])
    cylinder(12.5, d=7, center=true);
}

module gear() {
  color([1, .8, .5]) {
    difference() {
      union() {
        cylinder(12, d=9, center=true);
        translate([0, 0, 3])
          cylinder(6, d=11.5, center=true);
      }
      cylinder(19, d=5, center=true, $fn=24);
    }
  }
}

module pulley() {
  color([.8, .8, .8]) {
    difference() {
      cylinder(16, d=16, center=true);
      translate([0, 0, -3.4]) {
        difference() {
          cylinder(7.2, d=17, center=true);
          cylinder(7.2, d=12.25, center=true);
        }
      }
      cylinder(17, d=4, center=true, $fn=24);
    }
  }
}

module timing_roller() {
  color([.9, .9, .9]) {
    difference() {
      cylinder(9, d=6.8, center=true);
      cylinder(10, d=3.5, center=true);
    }
  }
}

module motor_and_gears() {  
  union() {
    //nema 11
    color([.2, .2, .2]) {
      cube([31.5, 28, 28], center=true);
      rotate([0, 90, 0])
        translate([0, 0, 25.5]) 
          cylinder(20, d=5, center=true);
    }
    
    //coupler
    color([1, .2, .2]) {
      rotate([0, 90, 0])
        translate([0, 0, 32.65]) 
          cylinder(20, d=12, center=true);
    }
    
    //3mm shaft
    color([.7, .7, .7]) {
      rotate([0, 90, 0])
        translate([0, 0, 60.75]) 
          cylinder(50, d=3, center=true);
    }
    rotate([0, 90, 0])
      translate([0, 0, 49.0]) 
        worm_gear();
    translate([50, 3, -8.6]) 
      rotate([90, 0, 0])
        gear();
    translate([50, -14, -8.6]) 
      rotate([90, 0, 0])
        pulley();
    translate([41, -10.5, -20])
      rotate([90, 0, 0])
        timing_roller();
    translate([58, -10.5, -20])
      rotate([90, 0, 0])
        timing_roller();
    
  }
}

module electronics() {
  color([0.1, .6, 0]) {
    cube([61, 41, 12]);
  }
}

module nema11_mount() {
  difference() {
    union() {
      difference() {
        cube([32, 32, 6], center=true);
        translate([0, 0, -2])
          cylinder(6, d=23, center=true, $fn=48);
        cylinder(8, d=7, center=true, $fn=48);
        translate([23.0/2, 23.0/2, 0])
          cylinder(8, d=2, center=true, $fn=24);
        translate([-23.0/2, 23.0/2, 0])
          cylinder(8, d=2, center=true, $fn=24);
        translate([23.0/2, -23.0/2, 0])
          cylinder(8, d=2, center=true, $fn=24);
        translate([-23.0/2, -23.0/2, 0])
          cylinder(8, d=2, center=true, $fn=24);
        
        //shaft slot
        translate([-10, 0, 0])
          cube([16, 5, 8], center=true);
        
        
      }
      translate([0, 17, -13])
        cube([32, 5, 32], center=true);
      
      //motor bed
      translate([16.3, 0, -15.75])
        rotate([0,90, 0])
          cube([31.5 + 6, 32, 4], center=true);
    }
    //sheath spot
    translate([14, rail_distance / 2 - 6, 2.25]) {
      rotate([0, -90, 0])
        sheath();
      translate([0, -1])
        sheath_hole();
    }
    
    //screw head spot in bed
    translate([12, 0, -18])
      rotate([0, 90, 0])
        cylinder(8, d=5, center=true, $fn=24);
  }
}





module gear_mount_half(trim=false) {
  difference() {
    translate([0, -0.5, 0])
      cube([20, 7.5, 9.5], center=true);
    translate([0, 0, 4.75])
      rotate([90, 0, 0])
        cylinder(10, d=4.1, center=true, $fn=24);
    
    //screw holes for #4 machine screws
    translate([-6, 0, 0])
      cylinder(10, d=2.5, center=true, $fn=24);
    translate([6, 0, 0])
      cylinder(10, d=2.5, center=true, $fn=24);
    
    if (trim) {
      translate([0, 7.5, 7])
        cube([22, 10, 10], center=true);
    }
  }
}

module holes_for_roller_mound() {
  rotate([90, 0, 0]) {
    cylinder(24, d=3.1, center=true, $fn=24);
    translate([17, 0, 0])
      cylinder(24, d=3.1, center=true, $fn=24);
  }
  
  //screw holes for #4 machine screws
  translate([8.5, 4, -2])
    cylinder(12, d=2.5, center=true, $fn=24);
  translate([8.5, 12, -2])
    cylinder(12, d=2.5, center=true, $fn=24);
}

module roller_mount_bottom() {
  difference() {
    cube([25, 13, 5], center=true);
    translate([-8.5, -8, 2.5])
      holes_for_roller_mound();
  }
}



module car_floor() {
  difference() {
    union() {
      //floor cube
      translate([-3, 0, -6]) cube([92, rail_distance - 6, 4], center=true);

      //sheaths
      translate([-3, rail_distance / 2 * -1, 0]) sheath();
      translate([-3, rail_distance / 2, 0]) sheath();
      
      //mounds for timing belt roller shafts
      translate([25.5, -11.5, -6])
        cube([26, 4, 6], center=true);
      translate([25.5, 7.5, -4.5])
        cube([26, 14, 3], center=true);
      
      //gear and pulley mounts
      translate([26, -21, 5.4]) {
        difference() {
          cube([20, 8, 19], center=true);
          rotate([90, 0, 0])
            cylinder(10, d=5.1, center=true, $fn=24);
        }
      }
      translate([26, 21, 5.4]) {
        translate([0, 0, -4.75])
          gear_mount_half();
        
        // assembled view only
        /*translate([0, 0, 4.75])
          rotate([0, 180, 0])
            gear_mount_half(true);*/
        
      }
      
      // assembled view only
      /*translate([-5.2, 6, 14])
        rotate([0, 90, 0])
          nema11_mount();*/
    }
    
    //inset for motor bed
      translate([-24 + 3, 6, -2.3])
        cube([31.5 + 6, 32, 4], center=true);
      
    
    //inset for roller shaft access
    translate([25.5, 7, -7.5])
      cube([26, 14, 3], center=true);
    
    //holes for roller shafts
    translate([17, -1, -6])
      holes_for_roller_mound();
    
    
    sheath_holes();
      
    //timing belt hole
    translate([25.5, -4.5, -6])
      cube([26, 10, 6], center=true);
  }
  
  // assembled view only
    //translate([25.5, 7, -8.5])
      //roller_mount_bottom();
}

module car_back_wall() {
  difference() {
    color([1, .6, .2])
      cube([4, rail_distance + 14, 42], center=true);
    translate([0, 0, -17]) {
      sheath_holes(true);
    }
  }
}

module car_front_wall() {
  difference() {
    color([1, .6, .2])
      cube([4, rail_distance + 14, 42], center=true);
    translate([0, 0, -17])
      sheath_holes();
    rotate([0, 90, 0])
      translate([3, 6, 0])
        cylinder(100, d=bearing_bore_diameter, center=true, $fn=24);
  }
}

module car_roof() {
  union() {
    difference() {
      color([1, .9, .2]) {
        union() {
          cube([92 + 9, rail_distance + 23, 26], center=true);
          
        }
      }
      translate([0, 0, -4])
        cube([93, rail_distance + 15, 26], center=true);
      
      //tripod screw nut spot
      translate([0, 0, 12])
        cylinder(6, d=12, center=true, $fn=48);
      
      //battery strap slot
      translate([0, rail_distance / -2 - 6, 11])
        cube([25, 2.5, 6], center=true);
      
      //wire hole for display
      translate([0, rail_distance / 2 + 6, 11])
        cube([25, 2.5, 6], center=true);
      
      //spare slots
      translate([40.75, 0, 11])
        cube([2.5, 20, 6], center=true);
      translate([-40.75, 0, 11])
        cube([2.5, 20, 6], center=true);
      
      //usb hole
      translate([13, rail_distance / -2 + 5, 11])
        cylinder(6, d=13, center=true, $fn=48);
    
      //button slots
      translate([-20, rail_distance / -2 + 5, 12])
        cube([12, 13, 3], center=true);
      translate([-4, rail_distance / -2 + 5, 12])
        cube([12, 13, 3], center=true);
      translate([-20, rail_distance / -2 - 0.75, 12])
        cube([6, 1.5, 8], center=true);
      translate([-20, rail_distance / -2 + 10.75, 12])
        cube([6, 1.5, 8], center=true);
        translate([-4, rail_distance / -2 - 0.75, 12])
        cube([6, 1.5, 8], center=true);
      translate([-4, rail_distance / -2 + 10.75, 12])
        cube([6, 1.5, 8], center=true);
        
      //tripod screw (1/4" - 20)
      cylinder(40, d=6.35, center=true, $fn=48);
      
      //screw holes (#8 size) top
      translate([34, rail_distance / 2, 10])
        cylinder(8, d=4.17, center=true, $fn=24);
      translate([34, rail_distance / -2, 10])
        cylinder(8, d=4.17, center=true, $fn=24);
      translate([-34, rail_distance / 2, 10])
        cylinder(8, d=4.17, center=true, $fn=24);
      translate([-34, rail_distance / -2, 10])
        cylinder(8, d=4.17, center=true, $fn=24);
        
      
      
    }
    difference() {
      //tripod head mount
      translate([0, 4.3, 8])
        cube([35, 29, 6], center=true);
      cylinder(40, d=6.35, center=true, $fn=48);
    }
  }
}

module car() {
  //printable stuff
  car_floor();
  *translate([-47, 0, 17])
    car_back_wall();
  translate([41, 0, 17])
    car_front_wall();
}

module rail_connectors() {
  cylinder(70, d=17.8);
  translate([20, 0, 0]) {
    cylinder(70, d=17.8);
  }
}

// modeled components (do not print)
*translate([-24, 6, 14])
  motor_and_gears();
*translate([-45, -10.3 ,-4])
  rotate([90, 0, 0])
    electronics();

*rails();

// individual printed components
translate([-5, 0, 29])
  rotate([180, 90, 90])
    car();
//translate([-3, 0, 29]) //installed view
translate([100, 0, -1]) rotate([180, 0, 0]) //print view
  car_roof();

translate([0, -70, -12])
  rotate([90, 90, 0])
    car_back_wall();

translate([-25, 65, 0]) gear_mount_half(true);
translate([-25, 80, 0]) roller_mount_bottom();
translate([-25, 40, 0])
  rotate([180, 0, 0])
    nema11_mount();

translate([5, 60, 0])
    rail_connectors();

translate([-80, 0, 8]) 
  rotate([0, 270, 180])
    mirror([1, 0, 0])
      foot();
translate([-130, 0, 8])
  rotate([0, 90, 0])
    foot();
