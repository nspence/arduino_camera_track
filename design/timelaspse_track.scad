rail_radius = 9;
rail_distance = 70;
bearing_radius = 8;
bearing_bore_diameter = 3;

module rail() {
  color([0.3, 0.3, 0.3])
  rotate([0, 90, 0]) cylinder(500, rail_radius, rail_radius, true);
}

module rails() {
  color([0.3, 0.3, 0.3]);
  translate([0, rail_distance / 2 * -1, 0]) rail();
  translate([0, rail_distance / 2, 0]) rail();
}

module foot() {
  difference() {
    cube([45, 80, 26], true);
    rails();
  }
}

module bearing_hole() {
  cube([5, 6, 14], true);
}

module bearing() {
  color([0.6, 0.6, 0.6])
  difference() {
    cylinder(4, r=5, center=true);
    cylinder(5, d=bearing_bore_diameter, center=true, $fn=24);
  }
}

module bearing_mount() {
  difference() {
    union() {
      translate([-1, 5, -1]) cube([20, 4, 14], true);
      translate([-1, -5, -1]) cube([20, 4, 14], true);
      translate([-14, 0, -1]) cube([10, 14, 14], true);
    }
    translate([5.25, 0, 0]) {
      rotate([90, 0, 0]) cylinder(16, d=bearing_bore_diameter, center=true, $fn=24);
    }
    
    //screw hole (#8 size)
    translate([-14, 0, 0])
      rotate([0, 90, 0])
        cylinder(14, d=4.17, center=true, $fn=24);
  }
  translate([5.25, 0, 0]) rotate([90, 0, 0]) bearing();
}

module sheath_hole() {
  cylinder(100, r=rail_radius + 0.5, center=true, $fn=50);
}

module sheath() {
  rotate([0, 90, 0]) {
    union() {
      difference() {
        cylinder(92, r=rail_radius + 3.5, center=true, $fn=50);
        sheath_hole();
        translate([-11.5, 0, 34]) bearing_hole();
        translate([-11.5, 0, -34]) bearing_hole();
      }
      
      translate([-19, 0, 34])
        rotate([180, 0, 0])
          bearing_mount();

      translate([-19, 0, -34]) bearing_mount();
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
  }
}


module sheath_holes() {
  translate([0, rail_distance / 2, 0])
    rotate([0, 90, 0]) 
      sheath_hole();
  translate([0, rail_distance / 2 * -1, 0])
    rotate([0, 90, 0])
      sheath_hole();
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
      translate([25.5, 2.5, -6])
        cube([26, 4, 6], center=true);
      
      //gear and pulley mounts
      translate([26, -21, 5.4]) {
        difference() {
          cube([20, 8, 19], center=true);
          rotate([90, 0, 0])
            cylinder(10, d=5, center=true, $fn=24);
        }
      }
      translate([26, 21, 5.4]) {
        difference() {
          cube([20, 8, 19], center=true);
          rotate([90, 0, 0])
            cylinder(10, d=4, center=true, $fn=24);
          translate([0, 0, 5])
            cube([20+1, 8+1, 9.5+.5], center=true);
        }
        difference() {
          translate([0, 0, 4.75])
            cube([20, 8, 9.5], center=true);
          rotate([90, 0, 0])
            cylinder(10, d=4, center=true, $fn=24);
        }
      }
      
      //motor bed
      translate([-24 + 3, 6, -2.0])
        cube([31.5 + 6, 32, 4], center=true);
      
      translate([-5.2, 6, 14])
        rotate([0, 90, 0])
          nema11_mount();
    }
    
    sheath_holes();
      
    //timing belt hole
    translate([25.5, -4.5, -6])
      cube([26, 10, 6], center=true);
  }
}

module car_back_wall() {
  difference() {
    color([1, .6, .2])
      cube([4, rail_distance + 14, 42], center=true);
    translate([0, 0, -17])
      sheath_holes();
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
  difference() {
    color([1, .9, .2])
      cube([92 + 8, rail_distance + 22, 26], center=true);
    translate([0, 0, -4])
      cube([92, rail_distance + 14, 26], center=true);
    
    //screw holes (#8 size)
    translate([34, 35, 10])
      cylinder(8, d=4.17, center=true, $fn=24);
    translate([34, -35, 10])
      cylinder(8, d=4.17, center=true, $fn=24);
    translate([-34, 35, 10])
      cylinder(8, d=4.17, center=true, $fn=24);
    translate([-34, -35, 10])
      cylinder(8, d=4.17, center=true, $fn=24);
  }
}

module car() {
  //printable stuff
  car_floor();
  *translate([-47, 0, 17])
    car_back_wall();
  *translate([41, 0, 17])
    car_front_wall();
  
  
  
  
}
module rail_connectors() {
  cylinder(70, d=18);
  translate([20, 0, 0]) {
    cylinder(70, d=18);
  }
}

// modeled components (do not print)
*translate([-24, 6, 14])
  motor_and_gears();
*translate([-45, -10.2 ,-4])
  rotate([90, 0, 0])
    electronics();

*rails();

// individual printed components
translate([0, 0, 0]) car();
//translate([-3, 0, 29]) //installed view
*translate([100, 0, 0]) rotate([180, 0, 0]) //print view
  car_roof();

*translate([0, 60, 0]) rail_connectors();

*translate([12, 0, 0]) foot();
*translate([512, 0, 0]) {
  mirror([1, 0, 0]) foot();
}
