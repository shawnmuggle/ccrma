class CreateTrees < ActiveRecord::Migration
  def self.up
    create_table :trees do |t|

      t.float :x
      t.float :y
      t.float :z
      
      t.float :age
      
      t.float :frequency
      
      t.references :planet

      t.timestamps
    end
  end

  def self.down
    drop_table :trees
  end
end